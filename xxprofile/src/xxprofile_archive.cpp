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
        fread(&fh, 1, sizeof(fh), _fp);
        if (fh.magic != kMagic) {
            fclose(_fp);
            _fp = NULL;
            return false;
        }
        _flags = fh.flags;
    }
    return _fp;
}

void Archive::flush() {
    if (_fp && _size) {
        fwrite(_buffer, 1, _size, _fp);
    }
    _size = 0;
}

void Archive::serialize(void* data, size_t size) {
    assert(_fp);
    if (!size) {
        return;
    }
    if (_write) {
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
    } else {
        fread(data, 1, size, _fp);
    }
}

XX_NAMESPACE_END(xxprofile);
