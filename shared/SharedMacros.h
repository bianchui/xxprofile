// Copyright 2018 bianchui. All rights reserved.
#ifndef __shared_SharedMacros_h__
#define __shared_SharedMacros_h__

#define CLASS_DELETE_COPY_CONSTRUCTOR(cls) \
/**/cls(const cls&) = delete; \

#define CLASS_DELETE_COPY_ASSIGN(cls) \
/**/cls& operator=(const cls&) = delete; \

#define CLASS_DELETE_COPY(cls) \
/**/cls(const cls&) = delete; \
/**/cls& operator=(const cls&) = delete; \

#define CLASS_DELETE_MOVE_CONSTRUCTOR(cls) \
/**/cls(const cls&&) = delete; \

#define CLASS_DELETE_MOVE_ASSIGN(cls) \
/**/cls& operator=(const cls&&) = delete; \

#define CLASS_DELETE_MOVE(cls) \
/**/cls(const cls&&) = delete; \
/**/cls& operator=(const cls&&) = delete; \

#define CLASS_DELETE_COPY_MOVE(cls) \
/**/CLASS_DELETE_COPY(cls); \
/**/CLASS_DELETE_MOVE(cls); \

#define ARRAY_COUNTOF(c) (sizeof(c) / sizeof(c[0]))

#ifdef NDEBUG

#  define DEBUGF(...)
#  define DEBUG_ONLY(...)

#else

#  define DEBUGF(...) printf(__VA_ARGS__)
#  define DEBUG_ONLY(...) __VA_ARGS__

#endif//DEBUG

#define NAMESPACE_BEGIN(ns) namespace ns {
#define NAMESPACE_END(ns) }

#endif//__shared_SharedMacros_h__
