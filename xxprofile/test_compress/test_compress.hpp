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

xxprofile::IDecompress* decompress_createZlib();
xxprofile::IDecompress* decompress_createLzo();
xxprofile::IDecompress* decompress_createLz4();

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

#endif /* test_compress_hpp */
