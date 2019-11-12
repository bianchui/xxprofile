// Copyright 2019 bianchui. All rights reserved.
#ifndef shared_platforms_windows_win_strings_hpp__
#define shared_platforms_windows_win_strings_hpp__
#include <string.h>

#define strcasecmp(a, b) _stricmp((a), (b))
#define strncasecmp(a, b, c) _strnicmp((a), (b), (c))

#endif//shared_platforms_windows_win_strings_hpp__
