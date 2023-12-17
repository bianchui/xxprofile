// Copyright 2017-2019 bianchui. All rights reserved.
#include "xxprofile_internal.hpp"
#include "xxprofile_archive.hpp"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

#define Archive_WriteBufferSize 0
#define Archive_ReadBufferSize (1024 * 1024)

Archive::Archive() {
    memset(this, 0, sizeof(Archive));
}

Archive::~Archive() {
    close();
}

bool Archive::open(const char* name, bool write) {
    assert(!_fp);
    if (_fp) {
        return false;
    }
    _fp = fopen(name, write ? "wb" : "rb");
    if (!_fp) {
        return false;
    }
    _write = write;
    SFileHeader fh = {0};
    static const char kMagic[] = "XPAR"; // XPAR = XxProfile ARchive
    if (_write) {
#if Archive_WriteBufferSize
        if (!_buffer) {
            _buffer = (char*)malloc(Archive_WriteBufferSize);
        }
#endif//Archive_WriteBufferSize
        memcpy(&fh.magic, kMagic, 4);
        if (sizeof(void*) == 8) {
            fh.flags |= Flag_pointer8;
        }
        fh.version = _version;
        fh.compressMethod = _compressMethod;
        fwrite(&fh, 1, sizeof(fh), _fp);
    } else {
#if Archive_ReadBufferSize
        if (!_buffer) {
            _buffer = (char*)malloc(Archive_ReadBufferSize);
        }
        _size = fread(_buffer, 1, Archive_ReadBufferSize, _fp);
        if (_size < sizeof(fh)) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
        memcpy(&fh, _buffer, sizeof(fh));
#else//Archive_ReadBufferSize
        fseek(_fp, 0, SEEK_END);
        _size = ftell(_fp);
        fseek(_fp, 0, SEEK_SET);
        if (sizeof(fh) != fread(&fh, 1, sizeof(fh), _fp)) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
#endif//Archive_ReadBufferSize
        _used = sizeof(fh);
        _filePointer = _used;
        if (memcmp(&fh.magic, kMagic, 4) != 0) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
        _version = fh.version;
        _flags = fh.flags;
        _compressMethod = fh.compressMethod;
    }
    return _fp;
}

void Archive::flush() {
    assert(_fp);
    assert(_write);
#if Archive_WriteBufferSize
    if (_fp && _write && _used) {
        fwrite(_buffer, 1, _used, _fp);
        _used = 0;
        //fflush(_fp);
    }
#endif//Archive_WriteBufferSize
}

void Archive::close() {
    if (_fp) {
#if Archive_WriteBufferSize
        if (_write && _used) {
            fwrite(_buffer, 1, _used, _fp);
        }
#endif//Archive_WriteBufferSize
        fclose(_fp);
    }
    if (_buffer) {
        free(_buffer);
    }
    memset(this, 0, sizeof(Archive));
}

bool Archive::eof() const {
#if Archive_ReadBufferSize
    return !_fp || ((!_write) && (_size < Archive_ReadBufferSize && _used >= _size));
#else//Archive_ReadBufferSize
    return !_fp || ((!_write) && (_used >= _size));
#endif//Archive_ReadBufferSize
}

void Archive::serialize(void* data, size_t size) {
    assert(_fp);
    if (!size || !_fp) {
        return;
    }
    if (_write) {
        XXDEBUG_DEBUG({
            if (size == 4) {
                printf("write %d(%d)\n", (int)size, *(int*)data);
            } else {
                printf("write %d\n", (int)size);
            }
        });
        _size += size;
#if Archive_WriteBufferSize
        if (size + _used >= Archive_WriteBufferSize) {
            if (_used) {
                const size_t writeSize = Archive_WriteBufferSize - _used;
                memcpy(_buffer + _used, data, writeSize);
                size -= writeSize;
                data = ((char*)data) + writeSize;
                fwrite(_buffer, 1, Archive_WriteBufferSize, _fp);
                _used = 0;
            }
            if (size > Archive_WriteBufferSize) {
                fwrite(data, 1, size, _fp);
                return;
            }
        }
        if (size) {
            memcpy(_buffer + _used, data, size);
            _used += size;
        }
#else//Archive_WriteBufferSize
        fwrite(data, 1, size, _fp);
#endif//Archive_WriteBufferSize
    } else {
        if (_error) {
            return;
        }
#if Archive_ReadBufferSize
        if (size + _used >= _size) {
            const size_t copySize = _size - _used;
            assert(copySize < 1000000);
            if (copySize == 0) {
                _error = true;
                _size = 0;
                _used = 0;
                return;
            }
            memcpy(data, _buffer + _used, copySize);
            size -= copySize;
            _filePointer += copySize;
            data = ((char*)data) + copySize;
            if (size > Archive_ReadBufferSize) {
                const size_t fullChunkSize = size - (size % Archive_ReadBufferSize);
                const size_t count = fread(data, 1, fullChunkSize, _fp);
                assert(count == fullChunkSize);
                if (count != fullChunkSize) {
                    _error = true;
                    _size = 0;
                    _used = 0;
                    return;
                }
                size -= fullChunkSize;
                data = ((char*)data) + fullChunkSize;
                _filePointer += fullChunkSize;
            }
            _size = fread(_buffer, 1, Archive_ReadBufferSize, _fp);
            _used = 0;
        }
        if (size) {
            size_t maxSize = _size - _used;
            //assert(size <= maxSize);
            if (size <= maxSize) {
                maxSize = size;
            } else {
                _error = true;
                printf("Archive read overflow at 0x%x(%d), read %d\n", (uint32_t)_filePointer, (uint32_t)_filePointer, (uint32_t)size);
            }
            if (maxSize) {
                memcpy(data, _buffer + _used, maxSize);
                _filePointer += maxSize;
            }
            _used += maxSize;
        }
#else//Archive_ReadBufferSize
        size_t count = fread(data, 1, size, _fp);
        _used += size;
        assert(count == size);
        if (count != size) {
            _error = true;
        }
#endif//Archive_ReadBufferSize
    }
}

XX_NAMESPACE_END(xxprofile);
