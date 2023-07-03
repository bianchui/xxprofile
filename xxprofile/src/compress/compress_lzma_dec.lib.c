// Copyright 2017-2023 bianchui. All rights reserved.
//#define XX_NO_DUMMY
#include "../../../libs/lzma/LzmaDec.c"

#ifdef __cplusplus
extern "C" {
#endif

SRes LzmaDec_SetPropAllocateProb(CLzmaDec *p, const CLzmaProps *propNew, ISzAlloc *alloc) {
    RINOK(LzmaDec_AllocateProbs2(p, propNew, alloc));
    p->prop = *propNew;
    return SZ_OK;
}

#ifdef __cplusplus
}
#endif
