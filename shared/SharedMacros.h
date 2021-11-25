// Copyright 2018 bianchui. All rights reserved.
#ifndef __shared_SharedMacros_h__
#define __shared_SharedMacros_h__
#include <stdlib.h>

#define CLASS_DELETE_COPY_CONSTRUCTOR(cls) \
/**/cls(const cls&) = delete; \

#define CLASS_DELETE_COPY_ASSIGN(cls) \
/**/cls& operator=(const cls&) = delete; \

#define CLASS_DELETE_COPY(cls) \
/**/cls(const cls&) = delete; \
/**/cls& operator=(const cls&) = delete; \

#define CLASS_DELETE_MOVE_CONSTRUCTOR(cls) \
/**/cls(cls&&) = delete; \

#define CLASS_DELETE_MOVE_ASSIGN(cls) \
/**/cls& operator=(cls&&) = delete; \

#define CLASS_DELETE_MOVE(cls) \
/**/cls(cls&&) = delete; \
/**/cls& operator=(cls&&) = delete; \

#define CLASS_DELETE_COPY_MOVE(cls) \
/**/CLASS_DELETE_COPY(cls); \
/**/CLASS_DELETE_MOVE(cls); \

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

// count __VA_ARGS__ param count, max 64 params
#define M_VA_ARGS_COUNT(...) M_VA_ARGS_COUNT_MAP_N_64(0, ##__VA_ARGS__,\
/**/64, 63, 62, 61, 60, \
/**/59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
/**/49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
/**/39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
/**/29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
/**/19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
/**/ 9,  8,  7,  6,  5,  4,  3,  2,  1,  0)
#define M_VA_ARGS_COUNT_MAP_N_64(\
/**/_0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, \
/**/_10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
/**/_20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
/**/_30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
/**/_40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
/**/_50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
/**/_60, _61, _62, _63, _64, N, ...) N

#define M_VA_MAP_0N(a0, aN, ...) M_VA_ARGS_COUNT_MAP_N_64(0, ##__VA_ARGS__, \
/**/aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, \
/**/aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, \
/**/aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, \
/**/aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, aN, \
/**/a0)

#define M_CONCAT__(x, a) x##a
#define M_CONCAT(x, a) M_CONCAT__(x, a)

#define M_EXPEND__(x) x
#define M_EXPEND(x) M_EXPEND__(x)

#define M_STR_EXPEND__(x) #x
#define M_STR_EXPEND(x) M_STR_EXPEND__(x)

#define SWITCH_CASE_TO_STRING(x) case x: return #x;

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

#if defined(__clang__)
// clang -E -dM - </dev/null
/* Clang/LLVM. ---------------------------------------------- */
#elif defined(__ICC) || defined(__INTEL_COMPILER)
/* Intel ICC/ICPC. ------------------------------------------ */
#elif defined(__GNUC__) || defined(__GNUG__)
// gcc -E -dM - </dev/null
/* GNU GCC/G++. --------------------------------------------- */
#elif defined(__HP_cc) || defined(__HP_aCC)
/* Hewlett-Packard C/aC++. ---------------------------------- */
#elif defined(__IBMC__) || defined(__IBMCPP__)
/* IBM XL C/C++. -------------------------------------------- */
#elif defined(_MSC_VER)
/* Microsoft Visual Studio. --------------------------------- */
#elif defined(__PGI)
/* Portland Group PGCC/PGCPP. ------------------------------- */
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
/* Oracle Solaris Studio. ----------------------------------- */
#endif

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

#ifdef __cplusplus
#  define C_EXTERN_C extern "C"
#else
#  define C_EXTERN_C extern
#endif

#ifndef _countof
#  ifdef __cplusplus
extern "C++" {
    template <typename _CountofType, size_t _SizeOfArray>
    char (*__countof_helper(UNALIGNED_DATA _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
}
#      define _countof(_Array) (sizeof(*__countof_helper(_Array)) + 0)
#  else
#      define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#  endif
#endif//_countof

#define CPPVER_CPLUSPLUS_11 (201103L)
#define CPPVER_CPLUSPLUS_14 (201402L)
#define CPPVER_CPLUSPLUS_17 (201703L)

#ifdef __cplusplus

#  if __cplusplus >= CPPVER_CPLUSPLUS_17
#    define CPLUSPLUS_NODISCARD [[nodiscard]]
#  endif

#endif//__cplusplus

#ifndef CPLUSPLUS_NODISCARD
#  define CPLUSPLUS_NODISCARD
#endif//CPLUSPLUS_NODISCARD

#ifdef __cplusplus
namespace shared {
    template <class Enum>
    struct EnumWrapper {
        Enum e;
        constexpr explicit operator bool() const noexcept {
            return (e != Enum{ 0 });
        }
        constexpr operator Enum() const noexcept {
            return e;
        }
    };

}
#endif//__cplusplus

# define DEFINE_BITMASK_ENUM(ENUM) \
/**/static_assert(std::is_enum<ENUM>::value, #ENUM " must be an enum type");                \
/**/CPLUSPLUS_NODISCARD                                                                     \
/**/inline constexpr shared::EnumWrapper<ENUM> operator &(ENUM lhs, ENUM rhs) noexcept {    \
/**/    typedef std::underlying_type<ENUM>::type U;                                         \
/**/    return shared::EnumWrapper<ENUM>{ENUM(static_cast<U>(lhs) & static_cast<U>(rhs))};  \
/**/}                                                                                       \
/**/CPLUSPLUS_NODISCARD                                                                     \
/**/inline constexpr shared::EnumWrapper<ENUM> operator |(ENUM lhs, ENUM rhs) noexcept {    \
/**/    typedef std::underlying_type<ENUM>::type U;                                         \
/**/    return shared::EnumWrapper<ENUM>{ENUM(static_cast<U>(lhs) | static_cast<U>(rhs))};  \
/**/}                                                                                       \
/**/CPLUSPLUS_NODISCARD                                                                     \
/**/inline constexpr shared::EnumWrapper<ENUM> operator ^(ENUM lhs, ENUM rhs) noexcept {    \
/**/    typedef std::underlying_type<ENUM>::type U;                                         \
/**/    return shared::EnumWrapper<ENUM>{ENUM(static_cast<U>(lhs) ^ static_cast<U>(rhs))};  \
/**/}                                                                                       \
/**/CPLUSPLUS_NODISCARD                                                                     \
/**/inline constexpr ENUM operator ~(ENUM value) noexcept {                                 \
/**/    typedef std::underlying_type<ENUM>::type U;                                         \
/**/    return ENUM(~static_cast<U>(value));                                                \
/**/}                                                                                       \
/**/inline constexpr ENUM& operator &=(ENUM& lhs, ENUM rhs) noexcept {                      \
/**/    return lhs = (lhs & rhs);                                                           \
/**/}                                                                                       \
/**/inline constexpr ENUM& operator |=(ENUM& lhs, ENUM rhs) noexcept {                      \
/**/    return lhs = (lhs | rhs);                                                           \
/**/}                                                                                       \
/**/inline constexpr ENUM& operator ^=(ENUM& lhs, ENUM rhs) noexcept {                      \
/**/    return lhs = (lhs ^ rhs);                                                           \
/**/}

#endif//__shared_SharedMacros_h__
