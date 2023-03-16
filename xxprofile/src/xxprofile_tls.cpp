// Copyright 2017 bianchui. All rights reserved.
#include "xxprofile_internal.hpp"
#include "xxprofile_tls.hpp"
#include "xxprofile_version.hpp"
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#if XX_IsCompress(ZLIB) || XX_IsCompress(ZLIB_CHUNKED)
#  include "compress/compress_zlib.cpp.h"
#elif XX_IsCompress(LZO) || XX_IsCompress(LZO_CHUNKED)
#  include "compress/compress_lzo.cpp.h"
#elif XX_IsCompress(LZ4) || XX_IsCompress(LZ4_CHUNKED)
#  include "compress/compress_lz4.cpp.h"
#elif XX_IsCompress(ZSTD) || XX_IsCompress(ZSTD_CHUNKED)
#  include "compress/compress_zstd.cpp.h"
#else
#  error compress method
#endif//

XX_NAMESPACE_BEGIN(xxprofile);

#if XX_IsCompress(ZLIB)
typedef SCompressZlib SCompress;
#elif XX_IsCompress(ZLIB_CHUNKED)
typedef SCompressChunkedZlib SCompress;
#elif XX_IsCompress(LZO)
typedef SCompressLzo SCompress;
#elif XX_IsCompress(LZO_CHUNKED)
typedef SCompressChunkedLzo SCompress;
#elif XX_IsCompress(LZ4)
typedef SCompressLz4 SCompress;
#elif XX_IsCompress(LZ4_CHUNKED)
typedef SCompressChunkedLz4 SCompress;
#elif XX_IsCompress(ZSTD)
typedef SCompressZstd SCompress;
#elif XX_IsCompress(ZSTD_CHUNKED)
typedef SCompressChunkedZstd SCompress;
#else
#  error compress method
#endif//

static std::atomic<uint32_t> g_frameId;

// SharedArchive
SharedArchive::SharedArchive(const char* path) {
    _archive.setVersion(EVersion::NOW);
    _archive.setCompressMethod(ECompressMethod::NOW);
    _compress = new SCompress();
    _compressBufferSize = _compress->calcBound(XXProfileTLS::ChunkByteSize);
    _compressBuffer = malloc(_compressBufferSize);

    _archive.open(path, true);
    Timer::InitTiming();
    double secondsPerCycle = Timer::GetSecondsPerCycle();
    _archive << secondsPerCycle;
}

SharedArchive::~SharedArchive() {
    _archive.close();
    delete _compress;
    free(_compressBuffer);
}

int SharedArchive::addRef() {
    const int ref = _refCount.fetch_add(1);
    assert(ref > 0);
    return ref + 1;
}

int SharedArchive::release() {
    const int ref = _refCount.fetch_sub(1) - 1;
    assert(ref >= 0);
    if (ref == 0) {
        delete this;
    }
    return ref;
}

// XXProfileTLS
void* XXProfileTLS::operator new(size_t size) {
    void* mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}
void XXProfileTLS::operator delete(void* p) {
    free(p);
}

XXProfileTLS::XXProfileTLS(SharedArchive* ar) {
    _threadId = systemGetTid();
    printf("Thread %d begin profile\n", _threadId);
    assert(ar);
    ar->addRef();
    _sharedAr = ar;

    _stack.reserve(100);
    _buffers.reserve(10);
    _freeBuffers.reserve(10);
}

XXProfileTLS::~XXProfileTLS() {
    assert(_stack.empty());
    if (_buffers.size()) {
        frameFlush();
    }
    for (auto iter = _freeBuffers.begin(); iter != _freeBuffers.end(); ++iter) {
        free(*iter);
    }
    _freeBuffers.clear();

    printf("Thread %d end profile\n", _threadId);
    _sharedAr->release();
}

XXProfileTreeNode* XXProfileTLS::beginScope(SName name) {
    if (!_currentBuffer || _usedCount == ChunkNodeCount) {
        _currentBuffer = newChunk();
        _usedCount = 0;
        _buffers.push_back(_currentBuffer);
    }

    XXProfileTreeNode* node = _currentBuffer + (_usedCount++);
    node->_beginTime = Timer::Cycles64();
    node->_name = name;
    node->_parentNodeId = _stack.empty() ? 0 : _stack.back().nodeId;
    _stack.push_back({node, ++_curNodeId});
    return node;
}

bool XXProfileTLS::endScope(XXProfileTreeNode* node) {
    assert(!_stack.empty());
    assert(_stack.back().node == node);
    node->_endTime = Timer::Cycles64();
    if (!_stack.empty()) {
        _stack.pop_back();
    }

    tryFrameFlush();
    return _stack.empty();
}

bool XXProfileTLS::increaseFrame() {
    // write buffers to disk
    ++g_frameId;
    //assert(_stack.empty());
    bool canFlush = _stack.empty();
    if (canFlush) {
        tryFrameFlush();
    }
    return canFlush;
}

XXProfileTreeNode* XXProfileTLS::newChunk() {
    XXProfileTreeNode* node = NULL;
    if (!_freeBuffers.empty()) {
        node = _freeBuffers.back();
        _freeBuffers.pop_back();
    } else {
        node = (XXProfileTreeNode*)malloc(ChunkNodeCount * sizeof(XXProfileTreeNode));
        //memset(node, 0, ChunkNodeCount * sizeof(XXProfileTreeNode));
    }
    return node;
}

void XXProfileTLS::tryFrameFlush() {
    if (_stack.empty()) {
        uint32_t frameId = g_frameId.load(std::memory_order_acquire);
        if (frameId > _frameId) {
            _frameId = frameId;
            frameFlush();
        }
    }
}

// uint32_t threadId; // from version 3
// uint32_t frameId;
// SName::Serialize();
// uint32_t nodeCount;
// XXProfileTreeNode nodes[nodeCount];
void XXProfileTLS::frameFlush() {
    SystemScopedLock lock(_sharedAr->lock());
    Archive& ar = _sharedAr->archive();
    ar << _threadId;
    ar << _frameId;
    XXLOG_DEBUG("frameFlush:frame %d\n", _frameId);
    SName::Serialize(&_tag, ar);
    uint32_t nodeCount = (uint32_t)_buffers.size();
    if (nodeCount) {
        nodeCount = (nodeCount - 1) * ChunkNodeCount + _usedCount;
    }
    ar << nodeCount;
    XXLOG_DEBUG("  nodeCount %d\n", nodeCount);
    void* compressBuffer = _sharedAr->compressBuffer();
    const size_t compressBufferSize = _sharedAr->compressBufferSize();
    ICompress& compress = *_sharedAr->compress();
    for (auto iter = _buffers.begin(); iter != _buffers.end(); ++iter) {
        XXProfileTreeNode* buffer = *iter;
        const size_t count = (buffer != _currentBuffer) ? ChunkNodeCount : _usedCount;
        uint32_t sizeOrg = (uint32_t)(count * sizeof(XXProfileTreeNode));
        const size_t compressedSize = compress.doCompress(compressBuffer, compressBufferSize, buffer, sizeOrg);
        uint32_t sizeCom = 0;
#if (XXCOMPRESS_NOW & XXCOMPRESS_CHUNKED_FLAG)
        // chunked always cannot drop any compressed data
        sizeCom = (uint32_t)compressedSize;
#else//if chunked
        // unchunked free to use smaller data
        if (compressedSize && compressedSize < sizeOrg) {
            sizeCom = (uint32_t)compressedSize;
        } else {
            assert(false);
        }
#endif//if chunked
        ar << sizeOrg;
        ar << sizeCom;
        if (sizeCom) {
            ar.serialize(compressBuffer, sizeCom);
        } else {
            ar.serialize(buffer, sizeOrg);
        }
        //memset(buffer, 0, sizeOrg);
        _freeBuffers.push_back(buffer);
    }
    ar.flush();

    // reset
    _usedCount = 0;
    _currentBuffer = NULL;
    _buffers.clear();
    _curNodeId = 0;
}

XX_NAMESPACE_END(xxprofile);
