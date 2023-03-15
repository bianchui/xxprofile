// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef compress_lz4_cpp_h
#define compress_lz4_cpp_h
#include <libs/lz4-1.9.4/lib/lz4.h>
#include <libs/lz4-1.9.4/lib/lz4hc.h>
#include <stdint.h>
#include "../xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressLz4 : ICompress {
    
    size_t calcBound(size_t size) {
        return LZ4_COMPRESSBOUND(size);
    }
    
    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
#ifndef XX_LZ4_USE_HC
        // fast 1 is best compression
        return LZ4_compress_fast((const char*)src, (char*)dst, (int)srcSize, (int)dstSize, 1);
#else//XX_LZ4_USE_HC
        // hc 1-9 is nearly same size
        return LZ4_compress_HC((const char*)src, (char*)dst, (int)srcSize, (int)dstSize, 1);
#endif//XX_LZ4_USE_HC
    }
    
};

#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressLz4 : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return LZ4_decompress_safe((const char*)src, (char*)dst, (int)srcSize, (int)dstSize);
    }
};
#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_lz4_cpp_h */
