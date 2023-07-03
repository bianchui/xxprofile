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

void printMK(char* buf, uint64_t size) {
    const uint64_t KB = 1024ll;
    const uint64_t MB = 1024ll * KB;
    if (size >= MB * 10) {
        sprintf(buf, "%.3fMB", size * 1.0 / MB);
    } else if (size >= KB * 10) {
        sprintf(buf, "%.3fKB", size * 1.0 / KB);
    } else {
        sprintf(buf, "%dB", (uint32_t)size);
    }
}

void testCompress(const char* name, const char* buf, xxprofile::ICompress* compress, xxprofile::IDecompress* decompress) {
    static const uint32_t kTestTimes = 100;
    char buffer[64];

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

    printMK(buffer, uint64_t(kTestTimes * kSrcSize / (end - start)));
    printf("%s: compress: %f %d => %d %s/s\n", name, end - start, kSrcSize, comLen, buffer);

    start = xxprofile::Timer::Seconds();

    for (uint32_t i = 0; i < kTestTimes; ++i) {
        decompress->doDecompress(dec, kSrcSize * 2, com, comLen);
    }

    end = xxprofile::Timer::Seconds();

    printMK(buffer, uint64_t(kTestTimes * kSrcSize / (end - start)));
    printf("%s: deccompress: %f %d => %d %s/s\n", name, end - start, comLen, decLen, buffer);

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

/*
 * Apple M1 Max
 * zlib_1: compress:      0.825550 | 786432 => 341099 | 90.849MB/s
 * zlib_1: deccompress:   0.249963 | 341099 => 786432 | 300.044MB/s
 * zlib_9: compress:      5.201362 | 786432 => 315490 | 14.419MB/s
 * zlib_9: deccompress:   0.236646 | 315490 => 786432 | 316.929MB/s
 * lz4_fast: compress:    0.101061 | 786432 => 436912 | 742.124MB/s
 * lz4_fast: deccompress: 0.015143 | 436912 => 786432 | 4952.865MB/s
 * zstd_1: compress:      0.130582 | 786432 => 352384 | 574.353MB/s
 * zstd_1: deccompress:   0.093797 | 352384 => 786432 | 799.596MB/s
 * zstd_3: compress:      0.270600 | 786432 => 319612 | 277.162MB/s
 * zstd_3: deccompress:   0.099471 | 319612 => 786432 | 753.989MB/s
 * zstd_6: compress:      0.655533 | 786432 => 306143 | 114.411MB/s
 * zstd_6: deccompress:   0.097566 | 306143 => 786432 | 768.711MB/s
 * zstd_15: compress:     2.453033 | 786432 => 303360 | 30.574MB/s
 * zstd_15: deccompress:  0.096327 | 303360 => 786432 | 778.602MB/s
 */

void readFile(char* buf, const char* name) {
    FILE* fp = fopen(name, "rb");
    if (fp) {
        fread(buf, 1, kSrcSize, fp);
        fclose(fp);
    }
}

int main(int argc, const char * argv[]) {
    xxprofile::Timer::InitTiming();
    char* buf = new char[kSrcSize];
    for (uint32_t i = 0; i < kSrcSize; ++i) {
        buf[i] = i + rand();
    }
    readFile(buf, argc > 1 ? argv[1] : argv[0]);
    if (true) {
        testCompress("zlib", buf, compress_createZlib(), decompress_createZlib());
        testCompress("lz4", buf, compress_createLz4(), decompress_createLz4());
        testCompress("zstd", buf, compress_createZstd(), decompress_createZstd());
    }
    if (true) {
        verifyCompress("zlib", buf, compress_createZlib(), decompress_createZlib());
        verifyCompress("zlibChunked", buf, compress_createChunkedZlib(), decompress_createChunkedZlib());
        verifyCompress("lz4", buf, compress_createLz4(), decompress_createLz4());
        verifyCompress("lz4Chunked", buf, compress_createChunkedLz4(), decompress_createChunkedLz4());
        verifyCompress("zstd", buf, compress_createZstd(), decompress_createZstd());
        verifyCompress("zstdChunked", buf, compress_createChunkedZstd(), decompress_createChunkedZstd());
    }
    delete[] buf;
    return 0;
}
