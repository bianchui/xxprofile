// Copyright 2017-2023 bianchui. All rights reserved.
#define DEBUGLEVEL 0
#include "../../../libs/zstd-1.5.5/lib/common/debug.c"
#include "../../../libs/zstd-1.5.5/lib/common/entropy_common.c"
#include "../../../libs/zstd-1.5.5/lib/common/error_private.c"
#include "../../../libs/zstd-1.5.5/lib/common/fse_decompress.c"
//#include "../../../libs/zstd-1.5.5/lib/common/pool.c"
//#include "../../../libs/zstd-1.5.5/lib/common/threading.c"
#include "../../../libs/zstd-1.5.5/lib/common/xxhash.c"
#include "../../../libs/zstd-1.5.5/lib/common/zstd_common.c"

#include "../../../libs/zstd-1.5.5/lib/compress/fse_compress.c"
#include "../../../libs/zstd-1.5.5/lib/compress/hist.c"
#include "../../../libs/zstd-1.5.5/lib/compress/huf_compress.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_compress_literals.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_compress_sequences.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_compress_superblock.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_compress.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_double_fast.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_fast.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_lazy.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_ldm.c"
#include "../../../libs/zstd-1.5.5/lib/compress/zstd_opt.c"
//#include "../../../libs/zstd-1.5.5/lib/compress/zstdmt_compress.c"

#ifdef XXPROFILE_HAS_DECOMPRESS
#include "../../../libs/zstd-1.5.5/lib/decompress/huf_decompress.c"
#include "../../../libs/zstd-1.5.5/lib/decompress/zstd_ddict.c"
#include "../../../libs/zstd-1.5.5/lib/decompress/zstd_decompress_block.c"
#include "../../../libs/zstd-1.5.5/lib/decompress/zstd_decompress.c"
#endif//XXPROFILE_HAS_DECOMPRESS
