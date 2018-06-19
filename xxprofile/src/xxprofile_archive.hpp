// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_archive_hpp
#define xxprofile_archive_hpp
#include "xxprofile_macros.hpp"
#include <stdio.h>

XX_NAMESPACE_BEGIN(xxprofile);

class Archive {
    enum { BufferSize = 1024 * 1024, };
private:
    FILE* _fp;
    size_t _size;
    size_t _used;
    char* _buffer;
    bool _write;

public:
    Archive();
    ~Archive();

    bool isWrite() const { return _write; }
    bool open(const char* name, bool write);
    void flush();

    void write(void* data, size_t size);
    void read(void* data, size_t size);


private:
    XX_CLASS_DELETE_COPY_AND_MOVE(Archive);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_archive_hpp
