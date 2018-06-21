// Copyright 2017 bianchui. All rights reserved.
#include "xxprofile_internal.hpp"
#include "xxprofile_tls.hpp"
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

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
    printf("Thread %d attached\n", _threadId);

    _stack.reserve(100);
    _buffers.reserve(10);
    _freeBuffers.reserve(10);
}

XXProfileTLS::~XXProfileTLS() {
    printf("Thread %d exit\n", _threadId);
}

XXProfileTreeNode* XXProfileTLS::beginScope(SName name) {
    if (!_currentBuffer || _usedCount == ChunkNodeCount) {
        _currentBuffer = newChunk();
        _usedCount = 0;
        _buffers.push_back(_currentBuffer);
    }

    XXProfileTreeNode* node = _currentBuffer + (_usedCount++);
    node->_beginTime = XXProfileTimer::Cycles64();
    node->_name = name;
    node->_nodeId = ++_curNodeId;
    node->_parentNodeId = _stack.empty() ? 0 : _stack.back()->_nodeId;
    _stack.push_back(node);
    return node;
}

void XXProfileTLS::endScope(XXProfileTreeNode* node) {
    assert(!_stack.empty());
    assert(_stack.back() == node);
    node->_endTime = XXProfileTimer::Cycles64();
    if (!_stack.empty()) {
        _stack.pop_back();
    }
}

bool XXProfileTLS::increaseFrame() {
    // write buffers to disk

    return true;
}

XXProfileTreeNode* XXProfileTLS::newChunk() {
    XXProfileTreeNode* node = NULL;
    if (!_freeBuffers.empty()) {
        node = _freeBuffers.back();
        _freeBuffers.pop_back();
    } else {
        node = (XXProfileTreeNode*)malloc(ChunkNodeCount * sizeof(XXProfileTreeNode));
    }
    memset(node, 0, ChunkNodeCount * sizeof(XXProfileTreeNode));
    return node;
}

XX_NAMESPACE_END(xxprofile);
