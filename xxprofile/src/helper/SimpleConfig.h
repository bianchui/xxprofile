//////////////////////////////////////////////////////////////////////////
// 文件: SimpleConfig.h
// 作者: BianChui
// 时间: 2011-07-19 16:39
// 说明: 
//////////////////////////////////////////////////////////////////////////
#ifndef SimpleConfig_h__
#define SimpleConfig_h__

#ifndef SIMPLE_SPACE
#  define SIMPLE_SPACE Simple
#endif//SIMPLE_SPACE

#define SIMPLE_SPACE_BEGIN	namespace SIMPLE_SPACE {
#define SIMPLE_SPACE_END	}

#ifdef _DEBUG
#  define SIMPLEARRAY_ACCEPTCHECK 1
#endif//_DEBUG

#ifndef SIMPLEARRAY_HAVE_ALIGNED_MALLOC
#  define SIMPLEARRAY_HAVE_ALIGNED_MALLOC 0
#endif//SIMPLEARRAY_HAVE_ALIGNED_MALLOC
#ifndef SIMPLECONFIG_HAVE_VA
#  define SIMPLECONFIG_HAVE_VA 0
#endif//SIMPLECONFIG_HAVE_VA
#define SIMPLEARRAY_THROW 0
#define SIMPLEARRAY_NEEDMORE 0
#define SIMPLEARRAY_HAVECOPY 0

#ifndef SIMPLESTRING_USE_WCHAR
#  define SIMPLESTRING_USE_WCHAR 0
#endif//SIMPLESTRING_USE_WCHAR

#endif//SimpleConfig_h__
