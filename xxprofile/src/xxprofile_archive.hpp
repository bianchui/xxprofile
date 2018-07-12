// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_archive_hpp
#define xxprofile_archive_hpp
#include "xxprofile_macros.hpp"
#include <stdio.h>

XX_NAMESPACE_BEGIN(xxprofile);

class Archive {
    enum { BufferSize = 1024 * 1024, };
    enum Flags {
        Flag_pointer8 = 1,
    };

private:
    struct SFileHeader {
        uint32_t magic;
        uint32_t version;
        uint32_t flags;
        uint32_t dummy;
    };

private:
    FILE* _fp;
    size_t _size;
    size_t _used;
    char* _buffer;
    uint32_t _version;
    uint32_t _flags;
    bool _write;
    bool _error;

public:
    Archive();
    ~Archive();

    void setVersion(uint32_t ver) { _version = ver; }
    uint32_t version() const { return _version; }
    bool isWrite() const { return _write; }
    bool hasError() const { return _error; }
    bool open(const char* name, bool write);
    void flush();
    void close();
    bool eof() const;

    struct Chunk {
        uint32_t chunkMagic;
        uint32_t chunkSize;
    };

    void serialize(void* data, size_t size);

    friend Archive& operator<<(Archive& ar, char& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, uint8_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, int8_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, uint16_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, int16_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, uint32_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, int32_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, uint64_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, int64_t& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, float& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

    friend Archive& operator<<(Archive& ar, double& value) {
        ar.serialize(&value, sizeof(value));
        return ar;
    }

private:
    XX_CLASS_DELETE_COPY_AND_MOVE(Archive);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_archive_hpp
