// Copyright 2017-2023 bianchui. All rights reserved.
#ifndef compress_zstd_cpp_h
#define compress_zstd_cpp_h
#include <libs/zstd-1.5.4/lib/zstd.h>

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressZstd : ICompress {

    size_t calcBound(size_t size) {
        return ZSTD_compressBound(size);
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return ZSTD_compress(dst, dstSize, src, srcSize, 1);
    }

};


#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressZstd : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return ZSTD_decompress(dst, dstSize, src, srcSize);
    }
};
#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_zstd_cpp_h */
