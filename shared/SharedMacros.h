// Copyright 2018 bianchui. All rights reserved.
#ifndef shared_SharedMacros_h__
#define shared_SharedMacros_h__

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

#define ARRAY_COUNTOF(c) (sizeof(c) / sizeof(c[0]))

#endif//shared_SharedMacros_h__
