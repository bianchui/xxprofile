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

XX_BEGIN_NAMESPACE;

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
        char buf[4];

        bool isEqual(const char* name) const {
            return strcmp(buf, name) == 0;
        }
    };
    
    SNamePool();
    ~SNamePool();

    uint32_t getNameId(const char* name);
    uint32_t getNameCount() const {
        return _nameCount;
    }
    FORCEINLINE const char* getName(uint32_t id);
    
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
		char* buffer = (char*)malloc(BUFFER_CHUNK_SIZE);
        _nameBuffers.push_back(buffer);
		memset(buffer, 0, BUFFER_CHUNK_SIZE);
        _buffer = make_align(buffer, NAME_ENTRY_ALIGN);
        _buffer_size = BUFFER_CHUNK_SIZE - (int32_t)(_buffer - buffer);
	}
	SNameEntry* entry = (SNameEntry*)_buffer;
	_buffer += size;
	_buffer_size -= (uint32_t)size;
	strcpy(entry->buf, name);
    
    uint32_t idx = _nameCount++;
    entry->id = idx + 1;
    if (idx == MAX_NAME_ENTRY_COUNT) {
        assert(false);
    }
    uint32_t chunkId = idx / NAMES_CHUNK_SIZE;
    uint32_t idxInChunk = idx % NAMES_CHUNK_SIZE;
    std::atomic<SNameEntry*>* chunk = _names[chunkId];
    if (!chunk) {
        chunk = (std::atomic<SNameEntry*>*)malloc(sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        memset(chunk, 0, sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        std::atomic<SNameEntry*>* expect = NULL;
        if (!_names[chunkId].compare_exchange_strong(expect, chunk)) {
            assert(false);
        }
    }
    SNameEntry* expEntry = NULL;
    if (!chunk[idxInChunk].compare_exchange_strong(expEntry, entry)) {
        assert(false);
    };
	return entry;
}

uint32_t SNamePool::getNameId(const char* name) {
    if (!name || !*name) {
        return 0;
    }
    const uint32_t hash = StringHash(name);
    const uint32_t bucket = hash % HASH_BUCKET_COUNT;
	for (SNameEntry* entry = _nameHashes[bucket]; entry; entry = entry->next) {
        if (entry->isEqual(name)) {
            return entry->id;
        }
    }
    CSystemScopedLock lock(_lock);
	for (SNameEntry* entry = _nameHashes[bucket]; entry; entry = entry->next) {
        if (entry->isEqual(name)) {
            return entry->id;
        }
    }
	SNameEntry* head = _nameHashes[bucket];
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

XX_END_NAMESPACE;
