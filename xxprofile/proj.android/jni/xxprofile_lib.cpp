#define ZSTDLIB_VISIBLE
#define ZSTDERRORLIB_VISIBILITY
#include "../../src/platforms/platform_base.cpp"
#include "../../src/platforms/platform_android.cpp"
#include "../../src/xxprofile_archive.cpp"
#include "../../src/xxprofile_name.cpp"
#include "../../src/xxprofile_tls.cpp"
#include "../../src/xxprofile.cpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#if XX_IsCompress(LZ4) || XX_IsCompress(LZ4_CHUNKED)
#  include "../../../libs/lz4-1.9.4/lib/lz4.c"
#  ifdef XX_LZ4_USE_HC
#    include "../../../libs/lz4-1.9.4/lib/lz4hc.c"
#  endif // XX_LZ4_USE_HC
#endif // XX_IsCompress(LZ4) || XX_IsCompress(LZ4_CHUNKED)

#if XX_IsCompress(ZSTD) || XX_IsCompress(ZSTD_CHUNKED)
#  include "../../src/compress/compress_zstd.lib.c"
#endif // XX_IsCompress(ZSTD) || XX_IsCompress(ZSTD_CHUNKED)

#pragma clang diagnostic pop
