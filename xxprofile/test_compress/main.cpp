//
//  main.cpp
//  test_compress
//
//  Created by bianchui on 2023/3/14.
//  Copyright © 2023 bianchui. All rights reserved.
//

#include <iostream>
#include "test_compress.hpp"
#include "../src/platforms/platform.hpp"
#include "../src/xxprofile_tls.hpp"

static const uint32_t kSrcSize = xxprofile::XXProfileTLS::ChunkByteSize;

void testCompress(const char* name, const char* buf, xxprofile::ICompress* compress, xxprofile::IDecompress* decompress) {
    static const uint32_t kTestTimes = 10000;

    char* com = new char[kSrcSize * 2];
    char* dec = new char[kSrcSize * 2];

    auto comLen = (uint32_t)compress->doCompress(com, kSrcSize * 2, buf, kSrcSize);
    auto decLen = (uint32_t)decompress->doDecompress(dec, kSrcSize * 2, com, comLen);

    printf("%s: check size: %s\n", name, decLen == kSrcSize ? "true" : "false");
    if (decLen == kSrcSize) {
        printf("%s: check: %s\n", name, memcmp(dec, buf, kSrcSize) == 0 ? "true" : "false");
    }

    auto start = xxprofile::Timer::Seconds();

    for (uint32_t i = 0; i < kTestTimes; ++i) {
        compress->doCompress(com, kSrcSize * 2, buf, kSrcSize);
    }

    auto end = xxprofile::Timer::Seconds();

    printf("%s: compress: %f %d => %d\n", name, end - start, kSrcSize, comLen);

    start = xxprofile::Timer::Seconds();

    for (uint32_t i = 0; i < kTestTimes; ++i) {
        decompress->doDecompress(dec, kSrcSize * 2, com, comLen);
    }

    end = xxprofile::Timer::Seconds();

    printf("%s: deccompress: %f %d => %d\n", name, end - start, comLen, decLen);

    delete[] com;
    delete[] dec;
}

void verifyCompress(const char* name, const char* buf, xxprofile::ICompress* compress, xxprofile::IDecompress* decompress) {
    char* com = new char[kSrcSize * 2];
    char* buf2 = new char[kSrcSize];
    srand(0);
    for (uint32_t i = 0; i < kSrcSize; ++i) {
        buf2[i] = rand();
    }
    for (uint32_t i = 0; i < kSrcSize; i += 2) {
        buf2[i] = i;
    }
    const char* buf3 = buf2;
    char* dec = new char[kSrcSize * 2];

    static const uint32_t kTestTImes = 10;

    for (uint32_t i = 0; i < kTestTImes; ++i) {
        const char* src = buf3 ? buf3 : buf;
        uint32_t size = rand() % kSrcSize;
        auto comLen = (uint32_t)compress->doCompress(com, kSrcSize * 2, src, size);
        auto decLen = (uint32_t)decompress->doDecompress(dec, kSrcSize * 2, com, comLen);

        printf("%s:[%d]", name, i);
        if (decLen == size) {
            if (memcmp(dec, src, size) == 0) {
                printf(" ok");
            } else {
                printf(" error: data");
            }
        } else {
            printf(" error: size: %d vs %d", decLen, size);
        }
        printf(" %d => %d\n", size, comLen);

        if (buf3) {
            buf3 = nullptr;
        } else {
            buf3 = buf2;
        }
    }

    if (buf2) {
        delete[] buf2;
    }
    delete[] com;
    delete[] dec;
}

int main(int argc, const char * argv[]) {
    xxprofile::Timer::InitTiming();
    char* buf = new char[kSrcSize];
    for (uint32_t i = 0; i < kSrcSize; ++i) {
        buf[i] = i;
    }
    if (false) {
        testCompress("zlib", buf, compress_createZlib(), decompress_createZlib());
        testCompress("lz4", buf, compress_createLz4(), decompress_createLz4());
        testCompress("zstd", buf, compress_createZstd(), decompress_createZstd());
    }
    if (true) {
        verifyCompress("zlib", buf, compress_createZlib(), decompress_createZlib());
        verifyCompress("zlibChunked", buf, compress_createChunkedZlib(), decompress_createChunkedZlib());
        verifyCompress("lz4", buf, compress_createZlib(), decompress_createZlib());
        verifyCompress("lz4Chunked", buf, compress_createChunkedLz4(), decompress_createChunkedLz4());
        verifyCompress("zstd", buf, compress_createZstd(), decompress_createZstd());
        verifyCompress("zstdChunked", buf, compress_createChunkedZstd(), decompress_createChunkedZstd());
    }
    delete[] buf;
    return 0;
}
