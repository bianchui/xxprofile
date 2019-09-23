// Copyright 2019 bianchui. All rights reserved.
#ifndef shared_platform_windows_inc_windows_h__
#define shared_platform_windows_inc_windows_h__

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#define NOMINMAX
#define NOSERVICE
#define NOMCX
#define NOIME

#include <windows.h>

// for winapifamily.h
#ifndef WINAPI_FAMILY_PARTITION
#  define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)
#endif//WINAPI_FAMILY_PARTITION

#ifndef WINAPI_PARTITION_DESKTOP
#  define WINAPI_PARTITION_DESKTOP 1
#  define WINAPI_PARTITION_APP 0
#  define WINAPI_PARTITION_PC_APP 0
#  define WINAPI_PARTITION_PHONE_APP 0
#endif//WINAPI_PARTITION_DESKTOP

#endif//shared_platform_windows_inc_windows_h__
