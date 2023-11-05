// Copyright 2017-2023 bianchui. All rights reserved.
#define XX_NO_DUMMY
#define XX_NO_LZMA2
#define XX_LOCAL_FUNCTION static
#define XX_FASTLOCAL_FUNCTION static
#define XX_FASTLZMA
#define _7ZIP_ST
#if defined(_MSC_VER) && (_MSC_VER > 1200)
#  define LZMA_LOG_BSR
#endif//defined(_MSC_VER) && (_MSC_VER > 1200)

#include "../../../libs/lzma/LzFind.c"
#include "../../../libs/lzma/LzmaEnc.c"
