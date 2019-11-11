#include "xxprofile_internal.hpp"
#include "xxprofile_archive.hpp"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

//#define Archive_BufferSize (1024 * 1024)
#define Archive_BufferSize 0

Archive::Archive() {
    memset(this, 0, sizeof(Archive));

#if Archive_BufferSize
    _buffer = (char*)malloc(Archive_BufferSize);
#endif//Archive_BufferSize
    _version = 1;
}

Archive::~Archive() {
    close();
#if Archive_BufferSize
    if (_buffer) {
        free(_buffer);
    }
#endif//Archive_BufferSize
}

bool Archive::open(const char* name, bool write) {
    assert(!_fp);
    _fp = fopen(name, write ? "wb" : "rb");
    if (!_fp) {
        return false;
    }
    _write = write;
    SFileHeader fh = {0};
    static const char kMagic[] = "XPAR"; // XPAR = XxProfile ARchive
    if (_write) {
        memcpy(&fh.magic, kMagic, 4);
        if (sizeof(void*) == 8) {
            fh.flags |= Flag_pointer8;
        }
        fh.version = _version;
        fwrite(&fh, 1, sizeof(fh), _fp);
    } else {
#if !Archive_BufferSize
        fseek(_fp, 0, SEEK_END);
        _size = ftell(_fp);
        fseek(_fp, 0, SEEK_SET);
        _used = sizeof(fh);
#endif//Archive_BufferSize
        fread(&fh, 1, sizeof(fh), _fp);
        if (memcmp(&fh.magic, kMagic, 4) != 0) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
        _version = fh.version;
        _flags = fh.flags;
#if Archive_BufferSize
        _size = fread(_buffer, 1, Archive_BufferSize, _fp);
#endif//Archive_BufferSize
    }
    return _fp;
}

void Archive::flush() {
    assert(_fp);
    assert(_write);
#if Archive_BufferSize
    if (_fp && _write && _used) {
        fwrite(_buffer, 1, _used, _fp);
        _used = 0;
        //fflush(_fp);
    }
#endif//Archive_BufferSize
}

void Archive::close() {
    if (_fp) {
#if Archive_BufferSize
        if (_write && _used) {
            fwrite(_buffer, 1, _used, _fp);
        }
#endif//Archive_BufferSize
        fclose(_fp);
    }
    auto buffer = _buffer;
    memset(this, 0, sizeof(Archive));
    _buffer = buffer;
}

bool Archive::eof() const {
#if Archive_BufferSize
    return !_fp || ((!_write) && (_size < Archive_BufferSize && _used >= _size));
#else//Archive_BufferSize
    return !_fp || ((!_write) && (_used >= _size));
#endif//Archive_BufferSize
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
#if Archive_BufferSize
        if (size + _used >= Archive_BufferSize) {
            if (_used) {
                const size_t writeSize = Archive_BufferSize - _used;
                memcpy(_buffer + _used, data, writeSize);
                size -= writeSize;
                data = ((char*)data) + writeSize;
                fwrite(_buffer, 1, Archive_BufferSize, _fp);
                _used = 0;
            }
            if (size > Archive_BufferSize) {
                fwrite(data, 1, size, _fp);
                return;
            }
        }
        if (size) {
            memcpy(_buffer + _used, data, size);
            _used += size;
        }
#else//Archive_BufferSize
        fwrite(data, 1, size, _fp);
#endif//Archive_BufferSize
    } else {
#if Archive_BufferSize
        if (size + _used >= _size) {
            const size_t readSize = _size - _used;
            memcpy(data, _buffer + _used, readSize);
            size -= readSize;
            data = ((char*)data) + readSize;
            if (size > Archive_BufferSize) {
                size_t count = fread(data, 1, size, _fp);
                assert(count == size);
                if (count != size) {
                    _error = true;
                }
                size = 0;
            }
            _size = fread(_buffer, 1, Archive_BufferSize, _fp);
            _used = 0;
        }
        if (size) {
            size_t maxSize = _size - _used;
            assert(size <= maxSize);
            if (size <= maxSize) {
                maxSize = size;
            } else {
                _error = true;
            }
            if (maxSize) {
                memcpy(data, _buffer + _used, maxSize);
            }
            _used += size;
        }
#else//Archive_BufferSize
        size_t count = fread(data, 1, size, _fp);
        _used += size;
        assert(count == size);
        if (count != size) {
            _error = true;
        }
#endif//Archive_BufferSize
    }
}

XX_NAMESPACE_END(xxprofile);
