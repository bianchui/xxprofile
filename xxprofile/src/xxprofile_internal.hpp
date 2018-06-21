// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_internal_h
#define xxprofile_internal_h

#include "platforms/platform.hpp"

#ifndef NDEBUG
#  define XXDEBUG_ONLY(x) x
#else//NDEBUG
#  define XXDEBUG_ONLY(x)
#endif//NDEBUG

#define XXDEBUG_ASSERT(e) assert(e)

#endif//xxprofile_internal_h
