//////////////////////////////////////////////////////////////////////////
// 文件: CommonMacros.h
// 作者: BianChui
// 时间: 2009-08-13 11:51
// 说明: 
//////////////////////////////////////////////////////////////////////////
#ifndef Helper_CommonMacros_h__
#define Helper_CommonMacros_h__

//////////////////////////////////////////////////////////////////////////
// ArrayTools
//////////////////////////////////////////////////////////////////////////
#ifndef _countof
#define _countof(x) \
	( sizeof(x) / sizeof((x)[0]) )
#endif//_countof

#define StringSetLastZero(x) \
	{ x[_countof(x)-1] = 0; }

//////////////////////////////////////////////////////////////////////////
// Bit mask macros
//////////////////////////////////////////////////////////////////////////
#define BM_IsFlag(x, f)			( !!((x)&(f)) )
#define BM_IsNotFlag(x, f)		( ((x)&(f)) == 0 )
#define BM_AllFlags(x, f)		( ((x)&(f)) == (f) )
#define BM_AddFlag(x, f)		( (x) |= (f) )
#define BM_RemoveFlag(x, f)		( (x) &= (~(f)) )
#define BM_SetFlag(x, f, b)		{ if (b) { BM_AddFlag(x, f); } else { BM_RemoveFlag(x, f); } }
#define BM_OtherFlags(x, f)		( ((x)&(~(f))) != 0 )
#define BM_MaskEqu(x, m, f)		( ((x)&(m)) == (f) )
#define BM_BitToBool(x)			( !!(x) )

#define ISPOW2(x)				( ((x)&((x)-1)) == 0 )

//////////////////////////////////////////////////////////////////////////
// Zero helper
//////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && 0
template<typename T> inline size_t SizeOfType(const T&) { return sizeof(T); }
#else//__cplusplus
#  define SizeOfType(x) sizeof(x)
#endif//__cplusplus
#define ZeroMemoryArray(x) memset((x), 0, SizeOfType(x))
#define ZeroMemoryArrayPointer(x, num) memset((x), 0, SizeOfType(*(x))*(num))
#define ZeroMemoryStruct(x) memset(&(x), 0, SizeOfType(x))
#define ZeroMemoryPointer(x) memset((x), 0, SizeOfType(*(x)))

#define CopyMemoryArray(x, y) memcpy((x), (y), SizeOfType(x))
#define CopyMemoryArrayPointer(x, y, num) memcpy((x), (y), SizeOfType(*(x))*(num))
#define CopyMemoryStruct(x) memcpy(&(x), &(y), SizeOfType(x))
#define CopyMemoryPointer(x) memcpy((x), (y), SizeOfType(*(x)))

//////////////////////////////////////////////////////////////////////////
// SafeMemoryHelper
//////////////////////////////////////////////////////////////////////////
#define SAFE_FREEPTR(x)		{ if (x) { free((void*)(x)); (x) = NULL; } }
#define SAFE_RELEASE(x)		{ if (x) { (x)->Release(); (x) = NULL; } }

//////////////////////////////////////////////////////////////////////////
// checkRet
//////////////////////////////////////////////////////////////////////////
#define FALSE_RET( x, v )		if( !( x ) ) { return v; }
#define TRUE_RET( x, v )		if( ( x ) ) { return v; }
#define FALSE_RETFALSE( x )		FALSE_RET( (x), FALSE )
#define TRUE_RETFALSE( x )		TRUE_RET( (x), FALSE )

//////////////////////////////////////////////////////////////////////////
// check
//////////////////////////////////////////////////////////////////////////

#define MESSAGE_NUMTOTEXT1(x) #x
#define MESSAGE_NUMTOTEXT(x) MESSAGE_NUMTOTEXT1(x)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

#endif//Helper_CommonMacros_h__
