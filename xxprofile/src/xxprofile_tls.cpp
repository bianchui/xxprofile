// Copyright 2017 bianchui. All rights reserved.
#include "xxprofile_internal.hpp"
#include "xxprofile_tls.hpp"
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <zlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

static std::atomic<uint32_t> g_frameId;

// XXProfileTLS
void* XXProfileTLS::operator new(size_t size) {
    void* mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}
void XXProfileTLS::operator delete(void* p) {
    free(p);
}

XXProfileTLS::XXProfileTLS(const char* path) {
    _threadId = systemGetTid();
    printf("Thread %d begin profile\n", _threadId);

    char name[PATH_MAX];
    sprintf(name, "%sThread_%d.xxprofile", path, _threadId);
    _ar.setVersion(2);
    _ar.open(name, true);

    double secondsPerCycle = Timer::GetSecondsPerCycle();
    _ar << secondsPerCycle;

    _stack.reserve(100);
    _buffers.reserve(10);
    _freeBuffers.reserve(10);

    _compressedBuf = (unsigned char*)newChunk();
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
    _ar.close();

    if (_compressedBuf) {
        free(_compressedBuf);
        _compressedBuf = NULL;
    }

    printf("Thread %d end profile\n", _threadId);
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

void XXProfileTLS::endScope(XXProfileTreeNode* node) {
    assert(!_stack.empty());
    assert(_stack.back().node == node);
    node->_endTime = Timer::Cycles64();
    if (!_stack.empty()) {
        _stack.pop_back();
    }

    tryFrameFlush();
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
        memset(node, 0, ChunkNodeCount * sizeof(XXProfileTreeNode));
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

// uint32_t frameId;
// SName::Serialize();
// uint32_t nodeCount;
// XXProfileTreeNode nodes[nodeCount];
void XXProfileTLS::frameFlush() {
    _ar << _frameId;
    XXLOG_DEBUG("frameFlush:frame %d\n", _frameId);
    SName::Serialize(&_tag, _ar);
    uint32_t nodeCount = (uint32_t)_buffers.size();
    if (nodeCount) {
        nodeCount = (nodeCount - 1) * ChunkNodeCount + _usedCount;
    }
    _ar << nodeCount;
    XXLOG_DEBUG("  nodeCount %d\n", nodeCount);
    for (auto iter = _buffers.begin(); iter != _buffers.end(); ++iter) {
        XXProfileTreeNode* buffer = *iter;
        const size_t count = (buffer != _currentBuffer) ? ChunkNodeCount : _usedCount;
        uint32_t sizeOrg = (uint32_t)(count * sizeof(XXProfileTreeNode));
        uLongf compressedSize = sizeOrg;
        uint32_t sizeCom = 0;
        if (Z_OK == compress2((Bytef*)_compressedBuf, &compressedSize, (const Bytef*)buffer, compressedSize, 9)) {
            sizeCom = (uint32_t)compressedSize;
        } else {
            assert(false);
        }
        _ar << sizeOrg;
        _ar << sizeCom;
        if (sizeCom) {
            _ar.serialize(_compressedBuf, sizeCom);
        } else {
            _ar.serialize(buffer, sizeOrg);
        }
        memset(buffer, 0, sizeOrg);
        _freeBuffers.push_back(buffer);
    }
    _ar.flush();

    // reset
    _usedCount = 0;
    _currentBuffer = NULL;
    _buffers.clear();
    _curNodeId = 0;
}

XX_NAMESPACE_END(xxprofile);
