//
//  xxprofile_name.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/28.
//  Copyright 2017 bianchui. All rights reserved.
//

#include "xxprofile_name.hpp"
#include "platforms/platform.hpp"
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include "xxprofile_archive.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

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
    
    CSystemLock _lock;
    
    std::vector<void*> _nameBuffers;

	SNameEntry* newNameEntry(const char* name);

private:
    static FORCEINLINE uint32_t StringHash(const char* lpszStr);
};

FORCEINLINE uint32_t SNamePool::StringHash(const char* lpszStr) {
    // BKDR Hash Function
    uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    uint32_t hash = 0;
    while (*lpszStr) {
        hash = hash * seed + *lpszStr++;
    }
    return hash;
}

SNamePool::SNamePool() {
    memset(_nameHashes, 0, sizeof(_nameHashes));
    memset(_names, 0, sizeof(_names));
    _buffer = NULL;
    _buffer_size = 0;
    _nameCount = 0;
}

SNamePool::~SNamePool() {
    for (size_t i = 0; i < _nameBuffers.size(); ++i) {
        free(_nameBuffers[i]);
    }
    _nameBuffers.clear();
    _buffer_size = 0;
    _nameCount = 0;
}

SNamePool::SNameEntry* SNamePool::newNameEntry(const char* name) {
	const size_t len = strlen(name);
	const size_t size = make_align(offsetof(SNameEntry, buf) + len + 1, NAME_ENTRY_ALIGN);
	if (size > _buffer_size) {
        const uint32_t currentNameCount = _nameCount.load(std::memory_order_acquire);
        if (!_nameBuffers.empty()) {
            SChunkHeader* header = (SChunkHeader*)make_align((char*)_nameBuffers.back(), NAME_ENTRY_ALIGN);
            header->usedSize = (uint32_t)(_buffer - (char*)header);
            header->endId = currentNameCount;
        }
		char* buffer = (char*)malloc(BUFFER_CHUNK_SIZE);
        _nameBuffers.push_back(buffer);
		memset(buffer, 0, BUFFER_CHUNK_SIZE);
        _buffer = make_align(buffer, NAME_ENTRY_ALIGN) + sizeof(SChunkHeader);
        _buffer_size = BUFFER_CHUNK_SIZE - (int32_t)(_buffer - buffer);
	}
	SNameEntry* entry = (SNameEntry*)_buffer;
	_buffer += size;
	_buffer_size -= (uint32_t)size;
	memcpy(entry->buf, name, len);
    
    uint32_t idx = _nameCount++;
    entry->id = idx + 1;
    entry->length = (uint32_t)len;
    if (idx == MAX_NAME_ENTRY_COUNT) {
        assert(false);
    }
    uint32_t chunkId = idx / NAMES_CHUNK_SIZE;
    uint32_t idxInChunk = idx % NAMES_CHUNK_SIZE;
    std::atomic<SNameEntry*>* chunk = _names[chunkId].load(std::memory_order_acquire);
    if (!chunk) {
        chunk = (std::atomic<SNameEntry*>*)malloc(sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        memset(chunk, 0, sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        std::atomic<SNameEntry*>* expect = NULL;
        if (!_names[chunkId].compare_exchange_strong(expect, chunk)) {
            assert(false);
        }
    }
    SNameEntry* expectEntry = NULL;
    if (!chunk[idxInChunk].compare_exchange_strong(expectEntry, entry)) {
        assert(false);
    };
	return entry;
}

uint32_t SNamePool::getNameId(const char* name) {
    if (!name || !*name) {
        return 0;
    }
    const uint32_t length = (uint32_t)strlen(name);
    const uint32_t hash = StringHash(name);
    const uint32_t bucket = hash % HASH_BUCKET_COUNT;
	for (SNameEntry* entry = _nameHashes[bucket].load(std::memory_order_acquire); entry; entry = entry->next) {
        if (entry->length == length && entry->isEqual(name)) {
            return entry->id;
        }
    }
    CSystemScopedLock lock(_lock);
	for (SNameEntry* entry = _nameHashes[bucket].load(std::memory_order_acquire); entry; entry = entry->next) {
        if (entry->length == length && entry->isEqual(name)) {
            return entry->id;
        }
    }
    SNameEntry* head = _nameHashes[bucket].load(std::memory_order_acquire);
	SNameEntry* newEntry = newNameEntry(name);
    newEntry->next = head;
    if (!_nameHashes[bucket].compare_exchange_strong(head, newEntry)) {
        assert(false);
    }
 
    return newEntry->id;
}

const char* SNamePool::getName(uint32_t id) {
    if (!id) {
        return "";
    }
    uint32_t idx = id - 1;
    uint32_t chunkId = idx / NAMES_CHUNK_SIZE;
    uint32_t idxInChunk = idx % NAMES_CHUNK_SIZE;
    std::atomic<SNameEntry*>* chunk = _names[chunkId].load(std::memory_order_relaxed);
    assert(chunk);
    SNameEntry* entry = chunk[idxInChunk].load(std::memory_order_relaxed);
    assert(entry);
    return entry->buf;
}

void SNamePool::serialize(SName::IncrementSerializeTag* tag, Archive& ar) {
    uint32_t chunkCount;
    if (ar.isWrite()) {
        uint32_t nameCount = _nameCount.load(std::memory_order_acquire);
        const uint32_t fromId = tag ? tag->fromId : 0;
        if (fromId == nameCount) {
            chunkCount = 0;
            ar << chunkCount;
            return;
        }
        uint32_t fromBufferIndex = 0;
        std::vector<void*> nameBuffers;
        const char* currentBufferEnd = NULL;
        {
            CSystemScopedLock lock(_lock);
            nameBuffers = _nameBuffers;
            currentBufferEnd = _buffer;
            // get second time to ensure
            nameCount = _nameCount.load(std::memory_order_acquire);
        }
        void** buffers = nameBuffers.data();
        const uint32_t bufferCount = (uint32_t)nameBuffers.size();
        if (fromId) {
            for (uint32_t iter = 1; iter < bufferCount; ++iter) {
                SChunkHeader* chunkHeader = (SChunkHeader*)make_align(buffers[bufferCount - iter - 1], NAME_ENTRY_ALIGN);
                if (chunkHeader->beginId() < fromId) {
                    break;
                }
                fromBufferIndex = bufferCount - iter - 1;
            }
        }
        chunkCount = bufferCount - fromBufferIndex;
        ar << chunkCount;
        uint32_t startIndex = fromId;
        for (uint32_t bufferIndex = fromBufferIndex; bufferIndex < bufferCount; ++bufferIndex) {
            SChunkHeader* chunkHeader = (SChunkHeader*)make_align(buffers[bufferIndex], NAME_ENTRY_ALIGN);
            const SNameEntry* firstEntry = chunkHeader->firstEntry();
            assert(firstEntry->id <= startIndex + 1);
            const char* chunkEnd = ((bufferIndex + 1) == bufferCount) ? currentBufferEnd : (((char*)chunkHeader) + chunkHeader->usedSize);
            uint32_t endIndex = ((bufferIndex + 1) == bufferCount) ? nameCount : chunkHeader->endId;

            const char* cur = (const char*)firstEntry;
            while (cur < chunkEnd) {
                const SNameEntry* entry = (SNameEntry*)cur;
                if (entry->id == startIndex + 1) {
                    break;
                }
                const size_t size = make_align(offsetof(SNameEntry, buf) + entry->length + 1, NAME_ENTRY_ALIGN);
                if (entry) {
                    cur += size;
                }
            }
            // uint32_t startIndex;
            // uint32_t endIndex;
            // uint32_t dataSize;
            // char data[dataSize];
            ar << startIndex << endIndex;
            uint32_t dataSize = (uint32_t)(chunkEnd - cur);
            ar << dataSize;
            ar.serialize((void*)cur, dataSize);
            startIndex = endIndex;
        }
        if (tag) {
            tag->fromId = nameCount;
        }
    } else {
        ar << chunkCount;
        if (chunkCount == 0) {
            return;
        }


        CSystemScopedLock lock(_lock);


    }
}

static SNamePool s_namePool;

// SName
SName::SName(const char* name) {
    id = s_namePool.getNameId(name);
}

SName::SName(uint32_t id) : id(id) {
    assert(c_str());
}

const char* SName::c_str() const {
    return s_namePool.getName(id);
}

void SName::Serialize(IncrementSerializeTag* tag, Archive& ar) {
    s_namePool.serialize(tag, ar);
}

XX_NAMESPACE_END(xxprofile);
