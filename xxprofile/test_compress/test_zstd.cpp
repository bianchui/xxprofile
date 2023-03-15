//
//  test_zstd.cpp
//  xxprofile
//
//  Created by bianchui on 2023/3/15.
//  Copyright © 2023 bianchui. All rights reserved.
//

#include "test_compress.hpp"
#include "../src/compress/compress_zstd.cpp.h"

IMPL_COMPRESS(Zstd);
IMPL_COMPRESS_CHUNKED(Zstd);
