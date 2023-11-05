//
//  test_lzma.cpp
//  xxprofile
//
//  Created by bianchui on 2023/7/3.
//  Copyright © 2023 bianchui. All rights reserved.
//

#include "test_compress.hpp"
#include "../src/compress/compress_lzma.cpp.h"

IMPL_COMPRESS(Lzma);
IMPL_COMPRESS_CHUNKED(Lzma);
