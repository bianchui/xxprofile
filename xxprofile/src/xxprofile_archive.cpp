#include "xxprofile_archive.hpp"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

Archive::Archive() {
    memset(this, 0, sizeof(Archive));
    if (BufferSize > 0) {
        _buffer = (char*)malloc(BufferSize);
    }
}

Archive::~Archive() {
    if (_fp) {
        if (_size) {
            fwrite(_buffer, 1, _size, _fp);
        }
        fclose(_fp);
    }
    if (_buffer) {
        free(_buffer);
    }
}

bool Archive::open(const char* name, bool write) {
    assert(!_fp);
    _fp = fopen(name, write ? "wb" : "rb");
    _write = write;
    return _fp;
}

void Archive::flush() {
    if (_fp && _size) {
        fwrite(_buffer, 1, _size, _fp);
    }
    _size = 0;
}

void Archive::write(void* data, size_t size) {
    assert(_write);
    assert(_fp);
    if (!size) {
        return;
    }
    size_t newSize = size + _size;
    if (newSize > BufferSize) {
        flush();
        if (size > BufferSize) {
            fwrite(data, 1, _size, _fp);
            return;
        }
    }
    if (size > 0) {
        memcpy(_buffer + _size, data, size);
        _size += size;
    }
}

void Archive::read(void* data, size_t size) {
    assert(!_write);
    assert(_fp);
    if (!size) {
        return;
    }
    fread(data, 1, size, _fp);
}

XX_NAMESPACE_END(xxprofile);
