// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef compress_zlib_cpp_h
#define compress_zlib_cpp_h
#include <zlib.h>
#include <stdint.h>
#include "../xxprofile_internal.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SCompressZlib : ICompress {
    
    size_t calcBound(size_t size) {
        return (size_t)deflateBound(nullptr, size) + 20;
    }
    
    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        uLongf dstLen = (uLongf)dstSize;
        bool success = Z_OK == compress2((Bytef *)dst, &dstLen, (const Bytef *)src, (uLong)srcSize, XXPROFILE_ZLIB_LEVEL);
        if (!success) {
            dstLen = 0;
        }
        return (size_t)dstLen;
    }

};

struct SCompressChunkedZlib : ICompress {
    z_stream stream;

    SCompressChunkedZlib() {
        memset(&stream, 0 , sizeof(stream));
        int err = deflateInit2(&stream, XXPROFILE_ZLIB_LEVEL, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
        assert(err == Z_OK);
    }

    ~SCompressChunkedZlib() {
        deflateEnd(&stream);
    }

    size_t calcBound(size_t size) {
        return (size_t)deflateBound(&stream, (uLong)size);
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        stream.next_in = (z_const Bytef *)src;
        stream.avail_in = (uInt)srcSize;
        stream.next_out = (Bytef *)dst;
        stream.avail_out = (uInt)dstSize;

        int err = deflate(&stream, Z_SYNC_FLUSH);
        assert(err == Z_OK);

        return dstSize - stream.avail_out;
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

struct SDecompressChunkedZlib : IDecompress {
    z_stream stream;

    SDecompressChunkedZlib() {
        memset(&stream, 0 , sizeof(stream));
        auto err = inflateInit2(&stream, MAX_WBITS);
        assert(err == Z_OK);
    }

    ~SDecompressChunkedZlib() {

    }
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        stream.next_in = (z_const Bytef *)src;
        stream.avail_in = (uInt)srcSize;
        stream.next_out = (Bytef *)dst;
        stream.avail_out = (uInt)dstSize;

        int err = inflate(&stream, Z_SYNC_FLUSH);
        assert(err == Z_OK);

        return dstSize - stream.avail_out;
    }
};

#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_zlib_cpp_h */
