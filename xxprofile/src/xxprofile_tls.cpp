// Copyright 2017 bianchui. All rights reserved.
#include "xxprofile_internal.hpp"
#include "xxprofile_tls.hpp"
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

static std::atomic<uint64_t> g_frameId;

// XXProfileTLS
void* XXProfileTLS::operator new(size_t size) {
    void* mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}
void XXProfileTLS::operator delete(void* p) {
    free(p);
}

XXProfileTLS::XXProfileTLS() {
    _threadId = GetTid();
    printf("Thread %d begin profile\n", _threadId);

    char name[PATH_MAX];
    sprintf(name, "Thread_%d.xxprofile", _threadId);
    _ar.open(name, true);

    _stack.reserve(100);
    _buffers.reserve(10);
    _freeBuffers.reserve(10);
}

XXProfileTLS::~XXProfileTLS() {
    assert(_stack.empty());
    frameFlush();
    for (auto iter = _freeBuffers.begin(); iter != _freeBuffers.end(); ++iter) {
        free(*iter);
    }
    _freeBuffers.clear();
    _ar.close();

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
    node->_nodeId = ++_curNodeId;
    node->_parentNodeId = _stack.empty() ? 0 : _stack.back()->_nodeId;
    _stack.push_back(node);
    return node;
}

void XXProfileTLS::endScope(XXProfileTreeNode* node) {
    assert(!_stack.empty());
    assert(_stack.back() == node);
    node->_endTime = Timer::Cycles64();
    if (!_stack.empty()) {
        _stack.pop_back();
    }

    tryFrameFlush();
}

bool XXProfileTLS::increaseFrame() {
    // write buffers to disk
    ++g_frameId;
    assert(_stack.empty());
    if (_stack.empty()) {
        frameFlush();
    }
    return _stack.empty();
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
        uint64_t frameId = g_frameId.load(std::memory_order_acquire);
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
    SName::Serialize(&_tag, _ar);
    uint32_t nodeCount = (uint32_t)_buffers.size();
    if (nodeCount) {
        nodeCount = (nodeCount - 1) * ChunkNodeCount + _usedCount;
    }
    _ar << nodeCount;
    for (auto iter = _buffers.begin(); iter != _buffers.end(); ++iter) {
        XXProfileTreeNode* buffer = *iter;
        const size_t count = (buffer != _currentBuffer) ? ChunkNodeCount : _usedCount;
        _ar.serialize(buffer, count * sizeof(XXProfileTreeNode));
        memset(buffer, 0, count * sizeof(XXProfileTreeNode));
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
