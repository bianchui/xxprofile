// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_tls_hpp
#define xxprofile_tls_hpp
#include "xxprofile.hpp"
#include "xxprofile_archive.hpp"
#include <vector>
#include "xxprofile_data.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

// XXProfileTLS
class XXProfileTLS : public XXProfile {
public:
    enum {
        ChunkNodeCount = 32 * 1024,
    };

    static XXProfileTLS* Get();

public:
    XXProfileTLS(const char* path);
    ~XXProfileTLS();

    bool increaseFrame();
    XXProfileTreeNode* beginScope(SName name);
    void endScope(XXProfileTreeNode* node);

public:
    static void* operator new(size_t size);
    static void operator delete(void* p);

protected:
    XXProfileTreeNode* newChunk();
    void tryFrameFlush();
    void frameFlush();

private:
    std::vector<XXProfileTreeNode*> _stack;
    uint32_t _frameId;
    uint32_t _usedCount;
    uint32_t _threadId;
    uint32_t _curNodeId;
    SName::IncrementSerializeTag _tag;
    Archive _ar;

    // allocation
private:
    std::vector<XXProfileTreeNode*> _buffers;
    std::vector<XXProfileTreeNode*> _freeBuffers;
    XXProfileTreeNode* _currentBuffer;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_tls_hpp
