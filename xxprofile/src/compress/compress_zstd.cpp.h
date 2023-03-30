// Copyright 2017-2023 bianchui. All rights reserved.
#ifndef compress_zstd_cpp_h
#define compress_zstd_cpp_h
#include <libs/zstd-1.5.4/lib/zstd.h>
#include "../xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressZstd : ICompress {

    size_t calcBound(size_t size) {
        return ZSTD_compressBound(size);
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return ZSTD_compress(dst, dstSize, src, srcSize, XXPROFILE_ZSTD_LEVEL);
    }

};

struct SCompressChunkedZstd : ICompress {
    ZSTD_CCtx* ctx;

    SCompressChunkedZstd() {
        ctx = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel, XXPROFILE_ZSTD_LEVEL);
    }

    ~SCompressChunkedZstd() {
        ZSTD_freeCCtx(ctx);
    }

    size_t calcBound(size_t size) {
        return ZSTD_compressBound(size);
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        ZSTD_inBuffer input = { src, srcSize, 0 };
        ZSTD_outBuffer output = { dst, dstSize, 0 };
        size_t const err = ZSTD_compressStream2(ctx, &output, &input, ZSTD_e_flush);
        UNUSED(err);
        assert(!ZSTD_isError(err));
        assert(input.size == input.pos);
        return output.pos;
    }
};

#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressZstd : IDecompress {
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return ZSTD_decompress(dst, dstSize, src, srcSize);
    }
};

struct SDecompressChunkedZstd : IDecompress {
    ZSTD_DCtx* ctx;

    SDecompressChunkedZstd() {
        ctx = ZSTD_createDCtx();
    }

    ~SDecompressChunkedZstd() {
        ZSTD_freeDCtx(ctx);
    }
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        ZSTD_inBuffer input = { src, srcSize, 0 };
        ZSTD_outBuffer output = { dst, dstSize, 0 };
        size_t const err = ZSTD_decompressStream(ctx, &output, &input);
        UNUSED(err);
        assert(!ZSTD_isError(err));
        assert(input.size == input.pos);
        return output.pos;
    }
};

#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_zstd_cpp_h */
