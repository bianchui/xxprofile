// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef compress_lzo_cpp_h
#define compress_lzo_cpp_h
#include <lzo/lzo1x.h>
#include <stdint.h>
#include "../xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressLzo : ICompress {
    
    size_t calcBound(size_t size) {
        return size;
    }
    
    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return 0;
    }
    
};

#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressLzo : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return 0;
    }
};
#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_lzo_cpp_h */
