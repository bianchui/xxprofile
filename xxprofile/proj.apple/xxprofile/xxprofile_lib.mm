#include "../../src/platforms/platform_base.cpp"
#include "../../src/platforms/platform_apple.mm"
#ifdef XX_PLATFORM_IOS
#include "../../src/platforms/apple/apple_ios_WritablePath.mm"
#endif//XX_PLATFORM_IOS
#ifdef XX_PLATFORM_MACOS
#include "../../src/platforms/apple/apple_macos_WritablePath.mm"
#endif//XX_PLATFORM_MACOS
#include "../../src/xxprofile_archive.cpp"
#include "../../src/xxprofile_name.cpp"
#include "../../src/xxprofile_tls.cpp"
#include "../../src/xxprofile.cpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include "../../../libs/lz4-1.9.2/lib/lz4.c"
#ifdef XX_LZ4_USE_HC
#include "../../../libs/lz4-1.9.2/lib/lz4hc.c"
#endif//XX_LZ4_USE_HC

#pragma clang diagnostic pop
