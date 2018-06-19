//
//  xxprofile.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//

#include "xxprofile.hpp"
#include "platforms/platform.hpp"
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

static pthread_key_t g_profile_tls_key;
static pthread_once_t g_profile_init_once = PTHREAD_ONCE_INIT;
static __thread XXProfile* g_tls_profile;

// XXProfileTreeNode
struct XXProfileTreeNode {
    uint64_t _beginTime;
    uint64_t _endTime;
    SName _name;
    uint32_t _nodeId;
    uint32_t _parentNodeId;
};

// XXProfile
static void profile_on_thread_exit(void* data) {
    XXProfile* profile = (XXProfile*)data;
    delete profile;
}

static void profile_tls_init_once() {
    pthread_key_create(&g_profile_tls_key, profile_on_thread_exit);
}

bool XXProfile::StaticInit() {
    pthread_once(&g_profile_init_once, profile_tls_init_once);
    return true;
}

XXProfile* XXProfile::Get() {
    if (!g_tls_profile) {
        XXProfile* profile = new XXProfile();
        pthread_setspecific(g_profile_tls_key, profile);
        g_tls_profile = profile;
    }
    return g_tls_profile;
}

void XXProfile::IncreaseFrame() {
    XXProfile* profile = Get();
    profile->increaseFrame();
}

XXProfile::XXProfile() : _currentBuffer(NULL) {
    //_stack.
}

XXProfileTreeNode* XXProfile::beginScope(SName name) {
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

void XXProfile::endScope(XXProfileTreeNode* node) {
    assert(!_stack.empty());
    assert(_stack.back() == node);
    node->_endTime = XXProfileTimer::Cycles64();
    if (!_stack.empty()) {
        _stack.pop_back();
    }
}

void XXProfile::increaseFrame() {
    // write buffers to disk
    
}

XXProfileTreeNode* XXProfile::newChunk() {
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
