// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_namepool_hpp
#define xxprofile_namepool_hpp
#include "xxprofile_internal.hpp"
#include <vector>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

class Archive;

class SNamePool {
public:
    enum {
        HASH_BUCKET_COUNT = 65536,

        NAME_ENTRY_ALIGN = sizeof(void*),
        BUFFER_CHUNK_SIZE = 512 * 1024,

        MAX_NAME_ENTRY_COUNT = 1 * 1024 * 1024,
        NAMES_CHUNK_SIZE = 16 * 1024,
        NAMES_CHUNK_COUNT = (MAX_NAME_ENTRY_COUNT + NAMES_CHUNK_SIZE - 1) / NAMES_CHUNK_SIZE,
    };

    struct SNameEntry {
        SNameEntry* next;//std::atomic<SNameEntry*>
        uint32_t id;
        uint32_t length;
        char buf[4];

        bool isEqual(const char* name) const {
            return strcmp(buf, name) == 0;
        }

        static size_t CalcEntrySize(size_t len) {
            return make_align(offsetof(SNameEntry, buf) + len + 1, NAME_ENTRY_ALIGN);
        }
    };

    struct SChunkHeader {
        uint32_t usedSize;
        uint32_t endId;

        SNameEntry* firstEntry() const {
            return (SNameEntry*)(((char*)this) + sizeof(SChunkHeader));
        }
        uint32_t beginId() const {
            return firstEntry()->id;
        }
    };

    SNamePool();
    ~SNamePool();

    void clear();

    uint32_t getNameId(const char* name);
    uint32_t getNameCount() const {
        return _nameCount.load(std::memory_order_acquire);
    }
    FORCEINLINE const char* getName(uint32_t id);

    void serialize(SName::IncrementSerializeTag* tag, Archive& ar);

private:// names
    std::atomic<SNameEntry*> _nameHashes[HASH_BUCKET_COUNT];
    std::atomic<std::atomic<SNameEntry*>*> _names[NAMES_CHUNK_COUNT];

private:// memory pool
    char* _buffer;
    uint32_t _buffer_size;
    std::atomic<uint32_t> _nameCount;

    SystemLock _lock;

    std::vector<void*> _nameBuffers;

    SNameEntry* newNameEntry(const char* name, const uint32_t length);

private:
    static FORCEINLINE uint32_t StringHash(const char* lpszStr);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_namepool_hpp
