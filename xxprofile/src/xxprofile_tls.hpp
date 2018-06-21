// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_tls_hpp
#define xxprofile_tls_hpp
#include "xxprofile.hpp"
#include <vector>

XX_NAMESPACE_BEGIN(xxprofile);

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
        ChunkNodeCount = 32 * 1024,
    };

    static XXProfileTLS* Get();

public:
    XXProfileTLS();
    ~XXProfileTLS();

    void increaseFrame();
    XXProfileTreeNode* beginScope(SName name);
    void endScope(XXProfileTreeNode* node);

public:
    static void* operator new(size_t size);
    static void operator delete(void* p);

protected:
    XXProfileTreeNode* newChunk();

private:
    uint64_t _frameId;
    std::vector<XXProfileTreeNode*> _stack;
    uint32_t _usedCount;
    uint32_t _threadId;
    uint32_t _curNodeId;

    // allocation
private:
    std::vector<XXProfileTreeNode*> _buffers;
    std::vector<XXProfileTreeNode*> _freeBuffers;
    XXProfileTreeNode* _currentBuffer;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_tls_hpp
