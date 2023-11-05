// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef xxprofile_internal_h
#define xxprofile_internal_h

#include "xxprofile_macros.hpp"
#include "platforms/platform.hpp"

#ifdef _MSC_VER
#  pragma warning(disable: 4068)
#endif//_MSC_VER

#ifndef NDEBUG
#  define XXDEBUG_ONLY(x) x
#else//NDEBUG
#  define XXDEBUG_ONLY(x)
#endif//NDEBUG

#define XXDEBUG_ASSERT(e) assert(e)

#define XXDEBUG_LEVEL_DEBUG 5
#define XXDEBUG_LEVEL_DETAIL 4
#define XXDEBUG_LEVEL_INFO 3
#define XXDEBUG_LEVEL_WARNING 2
#define XXDEBUG_LEVEL_ERROR 1
#define XXDEBUG_LEVEL_NONE 0

#define XXDEBUG_LEVEL XXDEBUG_LEVEL_INFO
#define XXLOG_LEVEL XXDEBUG_LEVEL_INFO

// XXDEBUG_LEVEL
#if XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DEBUG
#  define XXDEBUG_DEBUG(...) __VA_ARGS__
#else//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DEBUG
#  define XXDEBUG_DEBUG(...) {}
#endif//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DEBUG

#if XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DETAIL
#  define XXDEBUG_DETAIL(...) __VA_ARGS__
#else//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DETAIL
#  define XXDEBUG_DETAIL(...) {}
#endif//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_DETAIL

#if XXDEBUG_LEVEL >= XXDEBUG_LEVEL_INFO
#  define XXDEBUG_INFO(...) __VA_ARGS__
#else//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_INFO
#  define XXDEBUG_INFO(...) {}
#endif//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_INFO

#if XXDEBUG_LEVEL >= XXDEBUG_LEVEL_WARNING
#  define XXDEBUG_WARNING(...) __VA_ARGS__
#else//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_WARNING
#  define XXDEBUG_WARNING(...) {}
#endif//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_WARNING

#if XXDEBUG_LEVEL >= XXDEBUG_LEVEL_ERROR
#  define XXDEBUG_ERROR(...) __VA_ARGS__
#else//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_ERROR
#  define XXDEBUG_ERROR(...) {}
#endif//XXDEBUG_LEVEL >= XXDEBUG_LEVEL_ERROR

// XXLOG_LEVEL
#if XXLOG_LEVEL >= XXDEBUG_LEVEL_DEBUG
#  define XXLOG_DEBUG(...) printf(__VA_ARGS__)
#else//XXLOG_LEVEL >= XXDEBUG_LEVEL_DEBUG
#  define XXLOG_DEBUG(...) {}
#endif//XXLOG_LEVEL >= XXDEBUG_LEVEL_DEBUG

#if XXLOG_LEVEL >= XXDEBUG_LEVEL_DETAIL
#  define XXLOG_DETAIL(...) printf(__VA_ARGS__)
#else//XXLOG_LEVEL >= XXDEBUG_LEVEL_DETAIL
#  define XXLOG_DETAIL(...) {}
#endif//XXLOG_LEVEL >= XXDEBUG_LEVEL_DETAIL

#if XXLOG_LEVEL >= XXDEBUG_LEVEL_INFO
#  define XXLOG_INFO(...) printf(__VA_ARGS__)
#else//XXLOG_LEVEL >= XXDEBUG_LEVEL_INFO
#  define XXLOG_INFO(...) {}
#endif//XXLOG_LEVEL >= XXDEBUG_LEVEL_INFO

#if XXLOG_LEVEL >= XXDEBUG_LEVEL_WARNING
#  define XXLOG_WARNING(...) printf(__VA_ARGS__)
#else//XXLOG_LEVEL >= XXDEBUG_LEVEL_WARNING
#  define XXLOG_WARNING(...) {}
#endif//XXLOG_LEVEL >= XXDEBUG_LEVEL_WARNING

#if XXLOG_LEVEL >= XXDEBUG_LEVEL_ERROR
#  define XXLOG_ERROR(...) printf(__VA_ARGS__)
#else//XXLOG_LEVEL >= XXDEBUG_LEVEL_ERROR
#  define XXLOG_ERROR(...) {}
#endif//XXLOG_LEVEL >= XXDEBUG_LEVEL_ERROR

#define XXLOG(level, ...) XXLOG_##level(__VA_ARGS__)

#define XXPROFILE_ZLIB_LEVEL 1
#define XXPROFILE_ZSTD_LEVEL 1

XX_NAMESPACE_BEGIN(xxprofile);

struct ICompress {
    virtual ~ICompress() {}
    virtual size_t calcBound(size_t size) = 0;
    virtual size_t doCompress(void* dst, size_t dstSize, const void* src, size_t srcSize) = 0;
};

#ifdef XXPROFILE_HAS_DECOMPRESS

struct IDecompress {
    virtual ~IDecompress() {};
    virtual size_t doDecompress(void* dst, size_t dstSize, const void* src, size_t srcSize) = 0;
};

#endif//XXPROFILE_HAS_DECOMPRESS

XX_NAMESPACE_END(xxprofile);


#endif//xxprofile_internal_h
