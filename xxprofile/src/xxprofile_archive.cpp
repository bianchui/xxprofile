#include "xxprofile_archive.hpp"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

Archive::Archive() {
    memset(this, 0, sizeof(Archive));
    if (BufferSize) {
        _buffer = (char*)malloc(BufferSize);
    }
}

Archive::~Archive() {
    close();
    if (BufferSize && _buffer) {
        free(_buffer);
    }
}

bool Archive::open(const char* name, bool write) {
    assert(!_fp);
    _fp = fopen(name, write ? "wb" : "rb");
    if (!_fp) {
        return false;
    }
    _write = write;
    SFileHeader fh;
    static const uint32_t kMagic = 'RAPX'; // XPAR = XxProfile ARchive
    if (_write) {
        fh.magic = kMagic;
        fh.flags = 0;
        if (sizeof(void*) == 8) {
            fh.flags |= Flag_pointer8;
        }
        fh.version = 1;
        fwrite(&fh, 1, sizeof(fh), _fp);
    } else {
        if (!BufferSize) {
            fseek(_fp, 0, SEEK_END);
            _size = ftell(_fp);
            fseek(_fp, 0, SEEK_SET);
            _used = sizeof(fh);
        }
        fread(&fh, 1, sizeof(fh), _fp);
        if (fh.magic != kMagic) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
        _flags = fh.flags;
        if (BufferSize) {
            _size = fread(_buffer, 1, BufferSize, _fp);
        }
    }
    return _fp;
}

void Archive::flush() {
    assert(_fp);
    assert(_write);
    if (BufferSize && _fp && _write && _used) {
        fwrite(_buffer, 1, _used, _fp);
        _used = 0;
    }
}

void Archive::close() {
    if (_fp) {
        if (BufferSize && _write && _used) {
            fwrite(_buffer, 1, _used, _fp);
        }
        fclose(_fp);
    }
    auto buffer = _buffer;
    memset(this, 0, sizeof(Archive));
    _buffer = buffer;
}

bool Archive::eof() const {
    return !_fp || ((!_write) && (BufferSize ? (_size < BufferSize && _used >= _size) : (_used >= _size)));
}

void Archive::serialize(void* data, size_t size) {
    assert(_fp);
    if (!size) {
        return;
    }
    if (_write) {
        _size += size;
        if (BufferSize) {
            if (size + _used >= BufferSize) {
                if (_used) {
                    const size_t writeSize = BufferSize - _used;
                    memcpy(_buffer + _used, data, writeSize);
                    size -= writeSize;
                    data = ((char*)data) + writeSize;
                    fwrite(_buffer, 1, BufferSize, _fp);
                    _used = 0;
                }
                if (size > BufferSize) {
                    fwrite(data, 1, size, _fp);
                    return;
                }
            }
            if (size) {
                memcpy(_buffer + _used, data, size);
                _used += size;
            }
        } else {
            fwrite(data, 1, size, _fp);
        }
    } else {
        if (BufferSize) {
            if (size + _used >= _size) {
                const size_t readSize = _size - _used;
                memcpy(data, _buffer + _used, readSize);
                size -= readSize;
                data = ((char*)data) + readSize;
                if (size > BufferSize) {
                    size_t count = fread(data, 1, size, _fp);
                    assert(count == size);
                    size = 0;
                }
                _size = fread(_buffer, 1, BufferSize, _fp);
                _used = 0;
            }
            if (size) {
                size_t maxSize = _size - _used;
                assert(size <= maxSize);
                if (size < maxSize) {
                    maxSize = size;
                }
                if (maxSize) {
                    memcpy(data, _buffer + _used, maxSize);
                }
                _used += size;
            }
        } else {
            size_t count = fread(data, 1, size, _fp);
            _used += size;
            assert(count == size);
        }
    }
}

XX_NAMESPACE_END(xxprofile);
