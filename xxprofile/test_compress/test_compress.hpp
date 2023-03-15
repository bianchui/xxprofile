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

xxprofile::ICompress* compress_createZlib();
xxprofile::ICompress* compress_createLzo();
xxprofile::ICompress* compress_createLz4();
xxprofile::ICompress* compress_createZstd();

xxprofile::IDecompress* decompress_createZlib();
xxprofile::IDecompress* decompress_createLzo();
xxprofile::IDecompress* decompress_createLz4();
xxprofile::IDecompress* decompress_createZstd();

xxprofile::ICompress* compress_createChunkedZlib();
xxprofile::ICompress* compress_createChunkedLzo();
xxprofile::ICompress* compress_createChunkedLz4();
xxprofile::ICompress* compress_createChunkedZstd();

xxprofile::IDecompress* decompress_createChunkedZlib();
xxprofile::IDecompress* decompress_createChunkedLzo();
xxprofile::IDecompress* decompress_createChunkedLz4();
xxprofile::IDecompress* decompress_createChunkedZstd();

#define IMPL_COMPRESS(x) \
/**/xxprofile::ICompress* compress_create##x() {\
/**/    return new xxprofile::SCompress##x();\
/**/}\
/**/\
/**/xxprofile::IDecompress* decompress_create##x() {\
/**/    return new xxprofile::SDecompress##x();\
/**/}\
/**/

#define IMPL_COMPRESS_CHUNKED(x) \
/**/xxprofile::ICompress* compress_createChunked##x() {\
/**/    return new xxprofile::SCompressChunked##x();\
/**/}\
/**/\
/**/xxprofile::IDecompress* decompress_createChunked##x() {\
/**/    return new xxprofile::SDecompressChunked##x();\
/**/}\
/**/

#endif /* test_compress_hpp */
