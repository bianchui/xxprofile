//
//  test_memcpy.cpp
//  xxprofile
//
//  Created by bianchui on 2024/3/12.
//  Copyright © 2024 bianchui. All rights reserved.
//

#include "test_compress.hpp"
#include "../src/xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressMemcpy : ICompress {

    size_t calcBound(size_t size) {
        return size;
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        size_t size = dstSize < srcSize ? dstSize : srcSize;
        memcpy(dst, src, size);
        return size;
    }
};

typedef SCompressMemcpy SCompressChunkedMemcpy;

struct SDecompressMemcpy : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        size_t size = dstSize < srcSize ? dstSize : srcSize;
        memcpy(dst, src, size);
        return size;
    }
};

typedef SDecompressMemcpy SDecompressChunkedMemcpy;

XX_NAMESPACE_END(xxprofile);

IMPL_COMPRESS(Memcpy);
IMPL_COMPRESS_CHUNKED(Memcpy);
