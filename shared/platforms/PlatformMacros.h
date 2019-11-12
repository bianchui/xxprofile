// Copyright 2018 bianchui. All rights reserved.
#ifndef shared_platforms_PlatformMacros_h__
#define shared_platforms_PlatformMacros_h__
#include "PlatformTargets.h"

#ifdef __OBJC__
#define DECLARE_OBJECTIVE_C_CLASS_POINTER(cls) \
/**/@class cls; \
/**/typedef cls* cls##_ptr; \

#else//__OBJC__
#define DECLARE_OBJECTIVE_C_CLASS_POINTER(cls) \
/**/typedef void* cls##_ptr; \

#endif//__OBJC__

#endif//shared_platforms_PlatformMacros_h__
