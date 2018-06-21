//
//  xxprofile_name.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/28.
//  Copyright 2017 bianchui. All rights reserved.
//

#include "xxprofile_internal.hpp"
#include "xxprofile_name.hpp"
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include "xxprofile_archive.hpp"

#define XX_PROFILE_DEBUG_Name_Serialize 1

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

	SNameEntry* newNameEntry(const char* name, const uint32_t length);

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

SNamePool::SNameEntry* SNamePool::newNameEntry(const char* name, const uint32_t length) {
    assert(strlen(name) == length);
    const size_t size = SNameEntry::CalcEntrySize(length);
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
	memcpy(entry->buf, name, length);
    
    uint32_t idx = _nameCount++;
    entry->id = idx + 1;
    entry->length = length;
    if (idx == MAX_NAME_ENTRY_COUNT) {
        assert(false);
        abort();
    }
    uint32_t chunkId = idx / NAMES_CHUNK_SIZE;
    uint32_t idxInChunk = idx % NAMES_CHUNK_SIZE;
    std::atomic<SNameEntry*>* chunk = _names[chunkId].load(std::memory_order_acquire);
    if (!chunk) {
        chunk = (std::atomic<SNameEntry*>*)malloc(sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        memset(chunk, 0, sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
        std::atomic<SNameEntry*>* expect = NULL;
        if (!_names[chunkId].compare_exchange_strong(expect, chunk, std::memory_order_release)) {
            assert(false);
        }
    }
    SNameEntry* expectEntry = NULL;
    if (!chunk[idxInChunk].compare_exchange_strong(expectEntry, entry, std::memory_order_release)) {
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
    SNameEntry* head = _nameHashes[bucket].load(std::memory_order_acquire);
	for (SNameEntry* entry = head; entry; entry = entry->next) {
        if (entry->length == length && entry->isEqual(name)) {
            return entry->id;
        }
    }
	SNameEntry* newEntry = newNameEntry(name, length);
    newEntry->next = head;
    if (!_nameHashes[bucket].compare_exchange_strong(head, newEntry, std::memory_order_release)) {
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

// uint32_t nameCount;
// [
//     uint32_t startId;
//     {
//         uint32_t length;
//         char name[length];
//     }[nameCount];
// ]
void SNamePool::serialize(SName::IncrementSerializeTag* tag, Archive& ar) {
    uint32_t nameCount = 0;
    if (ar.isWrite()) {
        uint32_t maxNameId = _nameCount.load(std::memory_order_acquire);
        const uint32_t fromId = tag ? tag->fromId : 0;
        if (fromId >= maxNameId) {
            ar << nameCount;
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
            maxNameId = _nameCount.load(std::memory_order_acquire);
        }
        void** buffers = nameBuffers.data();
        const uint32_t bufferCount = (uint32_t)nameBuffers.size();
        if (fromId) {
            for (uint32_t iter = 1; iter < bufferCount; ++iter) {
                uint32_t index = bufferCount - iter;
                SChunkHeader* chunkHeader = (SChunkHeader*)make_align(buffers[index], NAME_ENTRY_ALIGN);
                if (chunkHeader->beginId() <= fromId) {
                    fromBufferIndex = index;
                    break;
                }
            }
        }
        nameCount = maxNameId - fromId;
        ar << nameCount;
        if (nameCount == 0) {
            return;
        }
        uint32_t startIndex = fromId;
        ar << startIndex;

        XXDEBUG_ONLY(uint32_t debug_max = fromId);
        XXDEBUG_ONLY(uint32_t debug_writeCount = 0);

        for (uint32_t bufferIndex = fromBufferIndex; bufferIndex < bufferCount; ++bufferIndex) {
            SChunkHeader* chunkHeader = (SChunkHeader*)make_align(buffers[bufferIndex], NAME_ENTRY_ALIGN);
            const SNameEntry* firstEntry = chunkHeader->firstEntry();
            assert(firstEntry->id <= startIndex + 1);
            const char* chunkEnd = ((bufferIndex + 1) == bufferCount) ? currentBufferEnd : (((char*)chunkHeader) + chunkHeader->usedSize);

            XXDEBUG_ONLY(uint32_t debug_id = firstEntry->id);
            const char* cur = (const char*)firstEntry;
            while (cur < chunkEnd) {
                const SNameEntry* entry = (SNameEntry*)cur;
                XXDEBUG_ASSERT(entry->id == debug_id);
                if (entry->id == startIndex + 1) {
                    break;
                }
                XXDEBUG_ONLY(++debug_id);
                cur += SNameEntry::CalcEntrySize(entry->length);
            }
            XXDEBUG_ASSERT(cur != chunkEnd);
            XXDEBUG_ONLY(uint32_t debug_index = startIndex);
            XXDEBUG_ASSERT(debug_id == debug_index + 1);
            while (cur < chunkEnd) {
                // uint32_t length
                // char name[length]
                SNameEntry* entry = (SNameEntry*)cur;
                XXDEBUG_ASSERT(entry->id == debug_index + 1);
                XXDEBUG_ASSERT(entry->id == debug_id);
#if XX_PROFILE_DEBUG_Name_Serialize
                ar << entry->id;
#endif//XX_PROFILE_DEBUG_Name_Serialize
                ar << entry->length;
                ar.serialize(entry->buf, entry->length);
                XXDEBUG_ONLY(debug_max = entry->id);
                XXDEBUG_ONLY(++debug_writeCount);
                cur += SNameEntry::CalcEntrySize(entry->length);
                if (entry->id == maxNameId) {
                    break;
                }
                XXDEBUG_ONLY(++debug_id);
                XXDEBUG_ONLY(++debug_index);
            }
            assert(cur == chunkEnd); // !Thread not safe!
            XXDEBUG_ASSERT(bufferIndex + 1 == bufferCount || chunkHeader->endId == debug_index);
            startIndex = chunkHeader->endId;
        }
        XXDEBUG_ASSERT(debug_writeCount == nameCount);
        XXDEBUG_ASSERT(debug_max == maxNameId);
        if (tag) {
            tag->fromId = maxNameId;
        }
    } else {
        ar << nameCount;
        if (nameCount == 0) {
            return;
        }
        uint32_t startIndex;
        ar << startIndex;
        std::string str;
        str.reserve(256);
        CSystemScopedLock lock(_lock);
        const uint32_t maxNameId = _nameCount.load(std::memory_order_acquire);
        std::atomic<SNameEntry*>* chunk = NULL;
        uint32_t currentChunkId = -1;
        XXDEBUG_ONLY(uint32_t debug_newMaxNameId = maxNameId);
        for (uint32_t i = 0; i < nameCount; ++i) {
#if XX_PROFILE_DEBUG_Name_Serialize
            uint32_t id2;
            ar << id2;
#endif//XX_PROFILE_DEBUG_Name_Serialize
            uint32_t length;
            ar << length;
            const uint32_t idx = startIndex + i;
            const uint32_t id = idx + 1;
            const uint32_t chunkId = idx / NAMES_CHUNK_SIZE;
            const uint32_t idxInChunk = idx % NAMES_CHUNK_SIZE;
            if (idx == MAX_NAME_ENTRY_COUNT) {
                assert(false);
                abort();
            }

#if XX_PROFILE_DEBUG_Name_Serialize
            assert(id == id2);
#endif//XX_PROFILE_DEBUG_Name_Serialize

            if (id < maxNameId) {
                str.resize(length);
                ar.serialize((void*)str.c_str(), length);
#ifndef NDEBUG
                // check only
                if (currentChunkId != chunkId) {
                    chunk = _names[chunkId].load(std::memory_order_acquire);
                    currentChunkId = chunkId;
                }
                assert(chunk);
                if (!chunk) {
                    return;
                }
                SNameEntry* entry = chunk[idxInChunk].load(std::memory_order_relaxed);
                assert(entry);
                if (!entry) {
                    return;
                }
                assert(entry->id == id);
                assert(entry->length == length);
                assert(strcmp(entry->buf, str.c_str()) == 0);
#endif//NDEBUG
            } else {
                const size_t size = SNameEntry::CalcEntrySize(length);
                if (size > _buffer_size) {
                    if (!_nameBuffers.empty()) {
                        SChunkHeader* header = (SChunkHeader*)make_align((char*)_nameBuffers.back(), NAME_ENTRY_ALIGN);
                        header->usedSize = (uint32_t)(_buffer - (char*)header);
                        header->endId = id - 1;
                    }
                    char* buffer = (char*)malloc(BUFFER_CHUNK_SIZE);
                    _nameBuffers.push_back(buffer);
                    memset(buffer, 0, BUFFER_CHUNK_SIZE);
                    _buffer = make_align(buffer, NAME_ENTRY_ALIGN) + sizeof(SChunkHeader);
                    _buffer_size = BUFFER_CHUNK_SIZE - (int32_t)(_buffer - buffer);
                }

                SNameEntry* newEntry = (SNameEntry*)_buffer;
                _buffer += size;
                _buffer_size -= (uint32_t)size;

                newEntry->id = id;
                newEntry->length = length;
                ar.serialize(newEntry->buf, length);

                {
                    const uint32_t hash = StringHash(newEntry->buf);
                    const uint32_t bucket = hash % HASH_BUCKET_COUNT;
                    SNameEntry* head = _nameHashes[bucket].load(std::memory_order_acquire);
#ifndef NDEBUG
                    for (SNameEntry* entry = head; entry; entry = entry->next) {
                        assert(entry->length != length || !entry->isEqual(newEntry->buf));
                    }
#endif//NDEBUG
                    newEntry->next = head;
                    if (!_nameHashes[bucket].compare_exchange_strong(head, newEntry, std::memory_order_release)) {
                        assert(false);
                    }
                }

                if (currentChunkId != chunkId) {
                    chunk = _names[chunkId].load(std::memory_order_acquire);
                    currentChunkId = chunkId;
                }
                if (!chunk) {
                    chunk = (std::atomic<SNameEntry*>*)malloc(sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
                    memset(chunk, 0, sizeof(SNameEntry*) * NAMES_CHUNK_SIZE);
                    std::atomic<SNameEntry*>* expect = NULL;
                    if (!_names[chunkId].compare_exchange_strong(expect, chunk, std::memory_order_release)) {
                        assert(false);
                    }
                }

                SNameEntry* expectEntry = NULL;
                if (!chunk[idxInChunk].compare_exchange_strong(expectEntry, newEntry, std::memory_order_release)) {
                    assert(false);
                };

                XXDEBUG_ASSERT(id == debug_newMaxNameId + 1);
                XXDEBUG_ONLY(debug_newMaxNameId = id);
                XXDEBUG_ASSERT(debug_newMaxNameId >= maxNameId);
            }
        }

        {
            uint32_t expectMaxNameId = maxNameId;
            uint32_t newMaxNameId = startIndex + nameCount;
            if (newMaxNameId > maxNameId) {
                if (!_nameCount.compare_exchange_strong(expectMaxNameId, newMaxNameId, std::memory_order_release)) {
                    assert(false);
                }
            } else {
                XXDEBUG_ONLY(newMaxNameId = maxNameId);
            }
            XXDEBUG_ASSERT(debug_newMaxNameId == newMaxNameId);
            XXDEBUG_ASSERT(debug_newMaxNameId >= maxNameId);
        }
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
