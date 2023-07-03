// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef compress_lzma_cpp_h
#define compress_lzma_cpp_h
#include <libs/lzma/LzmaDec.h>
#include <libs/lzma/LzmaEnc.h>
#include <stdint.h>
#include "../xxprofile_internal.hpp"

extern "C" SRes LzmaDec_SetPropAllocateProb(CLzmaDec *p, const CLzmaProps *propNew, ISzAlloc *alloc);

XX_NAMESPACE_BEGIN(xxprofile);

#define XX_LZMA_LEVEL 4
#define XX_LzmaMinDic 65536
#define XX_LZMA_MAX_DIC (4 * 1024 * 1024)
#define XX_LZMA_MAX_DIC1 (1 * 1024 * 1024)

struct LzmaMem {
    static void *LzmaAlloc(void*, size_t size) {
        return malloc(size);
    }

    static void LzmaFree(void*, void* address) {
        if (address != NULL){
            free(address);
        }
    }
};


static ISzAlloc g_lzmaAlloc = {LzmaMem::LzmaAlloc, LzmaMem::LzmaFree};

struct SCompressLzma : ICompress {

    size_t calcBound(size_t size) {
        return size * 2;
    }
    
    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        CLzmaEncProps props;
        LzmaEncProps_Init(&props);
        props.level = XX_LZMA_LEVEL;
        props.algo = 1;
        props.dictSize = XX_LZMA_MAX_DIC;

        CLzmaEncHandle p = LzmaEnc_Create(&g_lzmaAlloc);
        LzmaEnc_SetProps(p, &props);

        SizeT dstLen = dstSize;
        SRes res = LzmaEnc_MemEncode(p,
            (Byte *)dst, &dstLen, (const Byte *)src, srcSize,
            0, nullptr, &g_lzmaAlloc, &g_lzmaAlloc);
        assert(res == SZ_OK || res == SZ_ERROR_OUTPUT_EOF);
        LzmaEnc_Destroy(p, &g_lzmaAlloc, &g_lzmaAlloc);
        return (size_t)dstLen;
    }

};

struct SCompressChunkedLzma : ICompress {
    CLzmaEncProps _props;
    CLzmaEncHandle _enc;

    SCompressChunkedLzma() {
        _enc = nullptr;

        LzmaEncProps_Init(&_props);
        _props.level = XX_LZMA_LEVEL;
        _props.algo = 1;
        _enc = LzmaEnc_Create(&g_lzmaAlloc);
        assert(_enc);
    }

    ~SCompressChunkedLzma() {
        if (_enc) {
            LzmaEnc_Destroy(_enc, &g_lzmaAlloc, &g_lzmaAlloc);
        }
    }

    size_t calcBound(size_t size) {
        return size * 2;
    }

    size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        _props.dictSize = (UInt32)srcSize;
        if (_props.dictSize < XX_LzmaMinDic) {
            _props.dictSize = XX_LzmaMinDic;
        }
        if (LzmaEnc_SetProps(_enc, &_props) != SZ_OK) {
            assert(false);
            return false;
        }
        SizeT compressedSize = dstSize;
        SRes res = LzmaEnc_MemEncode(_enc,
            (Byte *)dst, &compressedSize, (const Byte *)src, srcSize,
            0, nullptr, &g_lzmaAlloc, &g_lzmaAlloc);
        assert(res == SZ_OK || res == SZ_ERROR_OUTPUT_EOF);
        return compressedSize;
    }
};

#ifdef XXPROFILE_HAS_DECOMPRESS
struct SDecompressLzma : IDecompress {
    Byte _props[10];
    SizeT _propSize;

    SDecompressLzma() {
        CLzmaEncProps props;
        LzmaEncProps_Init(&props);
        props.level = XX_LZMA_LEVEL;
        props.algo = 1;
        props.dictSize = XX_LZMA_MAX_DIC;

        CLzmaEncHandle p = LzmaEnc_Create(&g_lzmaAlloc);
        LzmaEnc_SetProps(p, &props);
        _propSize = sizeof(props);
        LzmaEnc_WriteProperties(p, _props, &_propSize);
        LzmaEnc_Destroy(p, &g_lzmaAlloc, &g_lzmaAlloc);
    }

    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        SizeT dstLen = dstSize;
        SizeT srcLen = srcSize;
        ELzmaStatus status;
        LzmaDecode((Byte*)dst, &dstLen, (const Byte*)src, &srcLen, _props, _propSize, LZMA_FINISH_ANY, &status, &g_lzmaAlloc);
        return (size_t)dstLen;
    }
};

struct SDecompressChunkedLzma : IDecompress {
    CLzmaDec _dec;

    SDecompressChunkedLzma() {
        _dec.probs = nullptr;
    }

    ~SDecompressChunkedLzma() {
        if (_dec.probs) {
            LzmaDec_FreeProbs(&_dec, &g_lzmaAlloc);
        }
        if (_dec.dic) {
            free(_dec.dic);
        }
    }
    size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        if (!_dec.probs) {
            CLzmaEncProps encProps;
            LzmaEncProps_Init(&encProps);
            encProps.level = XX_LZMA_LEVEL;
            encProps.algo = 1;
            encProps.dictSize = XX_LZMA_MAX_DIC;
            LzmaEncProps_Normalize(&encProps);
            CLzmaProps decProps;
            decProps.lc = (unsigned)encProps.lc;
            decProps.lp = (unsigned)encProps.lp;
            decProps.pb = (unsigned)encProps.pb;
            decProps.dicSize = encProps.dictSize;
            assert(decProps.dicSize == XX_LZMA_MAX_DIC);
            LzmaDec_Construct(&_dec);
            LzmaDec_SetPropAllocateProb(&_dec, &decProps, &g_lzmaAlloc);
        }
        _dec.dic = (Byte *)dst;
        _dec.prop.dicSize = _dec.dicBufSize = dstSize;
        LzmaDec_Init(&_dec);
        ELzmaStatus status;
        SizeT inSize = srcSize;
        SRes Result = LzmaDec_DecodeToDic(&_dec, dstSize, (const Byte*)src, &inSize, LZMA_FINISH_ANY, &status);
        return _dec.dicPos;
    }
};

#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);

#endif /* compress_lzma_cpp_h */
