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

class XXProfileTLS;

static pthread_key_t g_profile_tls_key;
static pthread_once_t g_profile_init_once = PTHREAD_ONCE_INIT;
static __thread XXProfileTLS* g_tls_profile;

// XXProfileTreeNode
struct XXProfileTreeNode {
    uint64_t _beginTime;
    uint64_t _endTime;
    SName _name;
    uint32_t _nodeId;
    uint32_t _parentNodeId;
};

// XXProfileTLS
class XXProfileTLS : public XXProfile {
public:
    enum {
        ChunkNodeCount = 256 * 1024,
    };

    static XXProfileTLS* Get();

public:
    XXProfileTLS();

    void increaseFrame();
    XXProfileTreeNode* beginScope(SName name);
    void endScope(XXProfileTreeNode* node);

protected:
    XXProfileTreeNode* newChunk();

private:
    uint64_t _frameId;
    std::vector<XXProfileTreeNode*> _stack;
    size_t _usedCount;
    uint32_t _curNodeId;

    // allocation
private:
    std::vector<XXProfileTreeNode*> _buffers;
    std::vector<XXProfileTreeNode*> _freeBuffers;
    XXProfileTreeNode* _currentBuffer;
};

// XXProfile
static void profile_on_thread_exit(void* data) {
    XXProfileTLS* profile = (XXProfileTLS*)data;
    delete profile;
}

static void profile_tls_init_once() {
    pthread_key_create(&g_profile_tls_key, profile_on_thread_exit);
    XXProfileTimer::InitTiming();
}

bool XXProfile::StaticInit() {
    pthread_once(&g_profile_init_once, profile_tls_init_once);
    return true;
}

XXProfileTLS* XXProfileTLS::Get() {
    if (!g_tls_profile) {
        XXProfileTLS* profile = new XXProfileTLS();
        pthread_setspecific(g_profile_tls_key, profile);
        g_tls_profile = profile;
    }
    return g_tls_profile;
}

void XXProfile::IncreaseFrame() {
    XXProfileTLS* profile = XXProfileTLS::Get();
    profile->increaseFrame();
}

// XXProfileTLS
XXProfileTLS::XXProfileTLS() : _currentBuffer(NULL) {
    _stack.reserve(100);
    _buffers.reserve(10);
    _freeBuffers.reserve(10);
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

void XXProfileTLS::increaseFrame() {
    // write buffers to disk
    
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

// XXProfileScope
XXProfileScope::XXProfileScope(const SName name) {
    _profile = XXProfileTLS::Get();
    _node = static_cast<XXProfileTLS*>(_profile)->beginScope(name);
}

XXProfileScope::~XXProfileScope() {
    static_cast<XXProfileTLS*>(_profile)->endScope(_node);
}

XX_NAMESPACE_END(xxprofile);
