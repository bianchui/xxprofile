//
//  test_compress.hpp
//  xxprofile
//
//  Created by bianchui on 2023/3/14.
//  Copyright © 2023 bianchui. All rights reserved.
//

#ifndef test_compress_hpp
#define test_compress_hpp
#define XXPROFILE_HAS_DECOMPRESS
#include "../src/xxprofile_internal.hpp"

#include <stdio.h>

class ICompress {
public:
    virtual ~ICompress() {
    }

    virtual size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) = 0;
};

ICompress* compress_createZlib();
ICompress* compress_createLzo();
ICompress* compress_createLz4();
ICompress* compress_createZstd();

xxprofile::IDecompress* decompress_createZlib();
xxprofile::IDecompress* decompress_createLzo();
xxprofile::IDecompress* decompress_createLz4();
xxprofile::IDecompress* decompress_createZstd();

ICompress* compress_createChunkedZlib();
ICompress* compress_createChunkedLzo();
ICompress* compress_createChunkedLz4();
ICompress* compress_createChunkedZstd();

xxprofile::IDecompress* decompress_createChunkedZlib();
xxprofile::IDecompress* decompress_createChunkedLzo();
xxprofile::IDecompress* decompress_createChunkedLz4();
xxprofile::IDecompress* decompress_createChunkedZstd();

#define IMPL_COMPRESS(x) \
/**/class CompressImpl##x : public ICompress {\
/**/    xxprofile::SCompress##x impl;\
/**/    virtual size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {\
/**/        return impl.doCompress(dst, dstSize, src, srcSize);\
/**/    }\
/**/};\
/**/\
/**/ICompress* compress_create##x() {\
/**/    return new CompressImpl##x();\
/**/}\
/**/\
/**/xxprofile::IDecompress* decompress_create##x() {\
/**/    return new xxprofile::SDecompress##x();\
/**/}\
/**/

#define IMPL_COMPRESS_CHUNKED(x) \
/**/class CompressImplChunked##x : public ICompress {\
/**/    xxprofile::SCompressChunked##x impl;\
/**/    virtual size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {\
/**/        return impl.doCompress(dst, dstSize, src, srcSize);\
/**/    }\
/**/};\
/**/\
/**/ICompress* compress_createChunked##x() {\
/**/    return new CompressImplChunked##x();\
/**/}\
/**/\
/**/xxprofile::IDecompress* decompress_createChunked##x() {\
/**/    return new xxprofile::SDecompressChunked##x();\
/**/}\
/**/

#endif /* test_compress_hpp */
