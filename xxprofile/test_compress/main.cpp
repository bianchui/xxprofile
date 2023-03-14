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

static const uint32_t kSrcSize = 128 * 1024;

void testCompress(const char* name, const char* buf, ICompress* compress, xxprofile::IDecompress* decompress) {
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

int main(int argc, const char * argv[]) {
    xxprofile::Timer::InitTiming();
    char* buf = new char[kSrcSize];
    for (uint32_t i = 0; i < kSrcSize; ++i) {
        buf[i] = i;
    }
    testCompress("zlib", buf, compress_createZlib(), decompress_createZlib());
    testCompress("lz4", buf, compress_createLz4(), decompress_createLz4());
    delete[] buf;
    return 0;
}
