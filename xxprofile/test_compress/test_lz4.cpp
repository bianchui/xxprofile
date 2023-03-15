//
//  test_lz4.cpp
//  xxprofile
//
//  Created by bianchui on 2023/3/14.
//  Copyright © 2023 bianchui. All rights reserved.
//

#include "test_compress.hpp"
#include "../src/compress/compress_lz4.cpp.h"

IMPL_COMPRESS(Lz4);
IMPL_COMPRESS_CHUNKED(Lz4);
