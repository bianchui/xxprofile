// Copyright 2017-2019 bianchui. All rights reserved.
#ifndef xxprofile_macros_hpp
#define xxprofile_macros_hpp
#include <stdint.h>

#define XX_CLASS_DELETE_COPY_CONSTRUCTOR(cls) \
/**/cls(const cls&) = delete; \

#define XX_CLASS_DELETE_COPY_ASSIGN(cls) \
/**/cls& operator=(const cls&) = delete; \

#define XX_CLASS_DELETE_COPY(cls) \
/**/XX_CLASS_DELETE_COPY_CONSTRUCTOR(cls); \
/**/XX_CLASS_DELETE_COPY_ASSIGN(cls); \

#define XX_CLASS_DELETE_MOVE_CONSTRUCTOR(cls) \
/**/cls(cls&&) = delete; \

#define XX_CLASS_DELETE_MOVE_ASSIGN(cls) \
/**/cls& operator=(cls&&) = delete; \

#define XX_CLASS_DELETE_MOVE(cls) \
/**/XX_CLASS_DELETE_MOVE_CONSTRUCTOR(cls); \
/**/XX_CLASS_DELETE_MOVE_ASSIGN(cls); \

#define XX_CLASS_DELETE_COPY_AND_MOVE(cls) \
/**/XX_CLASS_DELETE_COPY(cls); \
/**/XX_CLASS_DELETE_MOVE(cls); \

#define XX_ARRAY_COUNTOF(c) (sizeof(c) / sizeof(c[0]))

#define XX_NAMESPACE_BEGIN(ns) namespace ns {
#define XX_NAMESPACE_END(ns) }

// compilers
#ifdef _MSC_VER // for MSVC

#  define UNUSED(x) __pragma(warning(suppress:4100)) x
#  define UNUSED_VAR
#  define forceinline __forceinline
#  define STRUCT_PACKED __pragma(pack(1))
#  define CHECK_FMT(a, b)

#  if defined _M_X64 || defined _M_ARM || defined _M_ARM64
#    define UNALIGNED_DATA __unaligned
#  else
#    define UNALIGNED_DATA
#  endif

#elif defined __GNUC__ // for gcc/clang on Linux/Apple OS X

#  define UNUSED(x) (void)x
#  define UNUSED_VAR __attribute__((__unused__))
#  define forceinline __inline__ __attribute__((always_inline))
#  define STRUCT_PACKED __attribute__((packed))
#  define CHECK_FMT(a, b) __attribute__((format(printf, a, b)))

#  define UNALIGNED_DATA

#else

#  error unknown compiler
#  define UNUSED(x) (void)x
#  define UNUSED_VAR
#  define forceinline
#  define STRUCT_PACKED
#  define CHECK_FMT(a, b)

#  define UNALIGNED_DATA

#endif

#endif//xxprofile_macros_hpp
