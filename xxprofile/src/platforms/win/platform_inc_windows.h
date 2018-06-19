#ifndef xxprofile_win_platform_inc_windows_h__
#define xxprofile_win_platform_inc_windows_h__

#ifndef _WIN32_WINNT
# if !defined(WP8)
#  define _WIN32_WINNT 0x0500
# else//!defined(WP8)
#  define _WIN32_WINNT 0x0600
# endif//!defined(WP8)
#endif

#define NOGDICAPMASKS
#define NOMENUS
#define NORASTEROPS
#define NOATOM
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOCRYPT
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC

#ifdef _MSC_VER
// C4668: 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#  pragma warning(disable : 4668)
// C4820: 'bytes' bytes padding added after construct 'member_name'
#  pragma warning(disable : 4820)
// C4255: 'function' : no function prototype given: converting '()' to '(void)'
#  pragma warning(disable : 4255)

#  ifndef _DEBUG
// C4710: 'function' : function not inlined
#    pragma warning(disable : 4710)
// C4711: function 'function' selected for inline expansion
#    pragma warning(disable : 4711)
#  endif//_DEBUG

#endif//_MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif//xxprofile_win_platform_inc_windows_h__
