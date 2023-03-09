// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef compress_zlib_cpp_h
#define compress_zlib_cpp_h
#include <zlib.h>
#include <stdint.h>
#include "../xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressZlib {
    
    static size_t CalcCompressedSize(size_t size) {
        return size;
    }
    
    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        uLongf dstLen = (uLongf)dstSize;
        bool success = Z_OK == compress2((Bytef *)dst, &dstLen, (const Bytef *)src, (uLong)srcSize, 9);
        if (!success) {
            dstLen = 0;
        }
        return (size_t)dstLen;
    }
    
};

#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressZlib : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        uLongf destLen = (uLongf)dstSize;
        if (Z_OK != uncompress((Bytef *)dst, &destLen, (const Bytef *)src, (uLong)srcSize)) {
            destLen = 0;
        }
        return (size_t)destLen;
    }
};
#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_zlib_cpp_h */
