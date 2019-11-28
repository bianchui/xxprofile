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

#endif//xxprofile_macros_hpp
