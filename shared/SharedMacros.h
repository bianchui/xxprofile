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

#define CLASS_ZEROED_NEW() \
/**/static void* operator new(size_t size) { \
/**/    void* mem = malloc(size); \
/**/    memset(mem, 0, size); \
/**/    return mem; \
/**/} \
/**/static void operator delete(void* p) { \
/**/    free(p); \
/**/} \
/**/static void* operator new(size_t size, const std::nothrow_t&) { \
/**/    void* mem = malloc(size); \
/**/    memset(mem, 0, size); \
/**/    return mem; \
/**/} \
/**/static void operator delete(void* p, const std::nothrow_t&) { \
/**/    free(p); \
/**/} \
/**/static void* operator new(size_t size, void* p) { \
/**/    memset(p, 0, size); \
/**/    return p; \
/**/} \
/**/static void operator delete(void* p, void*) { \
/**/} \

#ifdef NDEBUG

#  define DEBUGF(...)
#  define DEBUG_ONLY(...)

#else

#  define DEBUGF(...) printf(__VA_ARGS__)
#  define DEBUG_ONLY(...) __VA_ARGS__

#endif//DEBUG

#define NAMESPACE_BEGIN(ns) namespace ns {
#define NAMESPACE_END(ns) }

#ifndef NS_SHARED
#  define NS_SHARED shared
#endif//NS_SHARED

#define SHARED_NAMESPACE_BEGIN NAMESPACE_BEGIN(NS_SHARED)
#define SHARED_NAMESPACE_END NAMESPACE_END(NS_SHARED)

#endif//__shared_SharedMacros_h__
