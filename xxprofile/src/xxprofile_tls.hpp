// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_tls_hpp
#define xxprofile_tls_hpp
#include "xxprofile.hpp"
#include "xxprofile_archive.hpp"
#include <vector>
#include <mutex>
#include "xxprofile_data.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class SharedArchive {
public:
    SharedArchive(const char* path);
    ~SharedArchive();
    
    int addRef();
    int release();
    
    void* compressBuffer() {
        return _compressBuffer;
    }
    
    size_t compressBufferSize() const {
        return _compressBufferSize;
    }
    
    std::mutex& lock() {
        return _mutex;
    }
    
    Archive& archive() {
        return _archive;
    }

    void markClose() {
        _pendingClose = 1;
    }

    bool isClosing() const {
        return _pendingClose > 0;
    }
    
private:
    Archive _archive;
    std::mutex _mutex;
    std::atomic_int _refCount = ATOMIC_VAR_INIT(1);
    std::atomic_int _pendingClose = ATOMIC_VAR_INIT(0);
    void* _compressBuffer;
    size_t _compressBufferSize;
};

// XXProfileTLS
class XXProfileTLS : public XXProfile {
public:
    enum {
        ChunkNodeCount = 32 * 1024,
        ChunkByteSize = ChunkNodeCount * sizeof(XXProfileTreeNode),
    };

    static XXProfileTLS* Get();
    static void Clear();

public:
    explicit XXProfileTLS(SharedArchive* ar);
    ~XXProfileTLS();

    bool increaseFrame();
    XXProfileTreeNode* beginScope(SName name);
    bool endScope(XXProfileTreeNode* node);

    bool isClosing() const {
        return _sharedAr->isClosing();
    }

public:
    static void* operator new(size_t size);
    static void operator delete(void* p);

protected:
    XXProfileTreeNode* newChunk();
    void tryFrameFlush();
    void frameFlush();

private:
    struct StackFrame {
        XXProfileTreeNode* node;
        uint32_t nodeId;
    };
    std::vector<StackFrame> _stack;
    uint32_t _frameId;
    uint32_t _usedCount;
    uint32_t _threadId;
    uint32_t _curNodeId;
    SName::IncrementSerializeTag _tag;
   
    SharedArchive* _sharedAr;

    // allocation
private:
    std::vector<XXProfileTreeNode*> _buffers;
    std::vector<XXProfileTreeNode*> _freeBuffers;
    XXProfileTreeNode* _currentBuffer;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_tls_hpp
