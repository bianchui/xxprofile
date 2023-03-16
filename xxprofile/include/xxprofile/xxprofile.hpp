//
//  xxprofile.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_hpp
#define xxprofile_hpp
#include <stdint.h>

#define NS_XXPROFILE       xxprofile
#define NS_XXPROFILE_BEGIN namespace NS_XXPROFILE {
#define NS_XXPROFILE_END   }

NS_XXPROFILE_BEGIN;

struct SName {
private:
    uint32_t _id;
    
public:
    SName(const char* name);
    SName(uint32_t id);
    const char* c_str() const;
    uint32_t id() const {
        return _id;
    }
};

class XXProfile {
public:
    static bool StaticInit(const char* savePath);
    static void StaticUninit();
    static bool IncreaseFrame();
};

class XXProfileScope {
public:
    XXProfileScope(const SName name);
    ~XXProfileScope();
    
private:
    void* _node;
    XXProfile* _profile;
    
private:
    XXProfileScope(const XXProfileScope&) = delete;
    XXProfileScope& operator=(const XXProfileScope&) = delete;
    XXProfileScope(XXProfileScope&&) = delete;
    XXProfileScope& operator=(XXProfileScope&&) = delete;
};

NS_XXPROFILE_END;

#if defined(_DEBUG) || defined(DEBUG)
#  ifndef XX_ENABLE_PROFILE
#    define XX_ENABLE_PROFILE 0
#  endif//XX_ENABLE_PROFILE
#endif//defined(_DEBUG) || defined(DEBUG)

#ifndef XX_ENABLE_PROFILE
#  define XX_ENABLE_PROFILE 0
#endif//XX_ENABLE_PROFILE

#if XX_ENABLE_PROFILE

#ifdef _MSC_VER
#  define HAVE_PRETTY_FUNCTION 0
#else//_MSC_VER
#  define HAVE_PRETTY_FUNCTION 1
#endif//_MSC_VER

#if HAVE_PRETTY_FUNCTION
#  define XX_PROFILE_FUNCTION __PRETTY_FUNCTION__
#endif//HAVE_PRETTY_FUNCTION

#ifndef XX_PROFILE_FUNCTION
#  define XX_PROFILE_FUNCTION __FUNCTION__
#endif//XX_PROFILE_FUNCTION

#  define XX_PROFILE_SCOPE_FUNCTION() \
/**/static ::NS_XXPROFILE::SName __xxprofile_name_function_name(XX_PROFILE_FUNCTION); \
/**/::NS_XXPROFILE::XXProfileScope __xxprofile_scope(__xxprofile_name_function_name); \

#  define XX_PROFILE_SCOPE_NAME(name) \
/**/static ::NS_XXPROFILE::SName __xxprofile_name_##name##_name(#name); \
/**/::NS_XXPROFILE::XXProfileScope __xxprofile_scope_##name(__xxprofile_name_##name##_name); \

#  define XX_PROFILE_SCOPE_DYNAMIC_NAME(name) \
/**/::NS_XXPROFILE::XXProfileScope __xxprofile_scope_##__LINE__##_##__COUNTER__((::xxprofile::SName((name)))); \

#  define XX_PROFILE_INCREASE_FRAME() \
/**/::NS_XXPROFILE::XXProfile::IncreaseFrame(); \

#  define XX_PROFILE_STATIC_INIT(savePath) \
/**/::NS_XXPROFILE::XXProfile::StaticInit(savePath); \

#  define XX_PROFILE_STATIC_UNINIT() \
/**/::NS_XXPROFILE::XXProfile::StaticUninit(); \

#  define XX_PROFILE_ENABLED_ONLY(x) x

#else//XX_ENABLE_PROFILE

#  define XX_PROFILE_SCOPE_FUNCTION()
#  define XX_PROFILE_SCOPE_NAME(name)
#  define XX_PROFILE_SCOPE_DYNAMIC_NAME(name)
#  define XX_PROFILE_INCREASE_FRAME()
#  define XX_PROFILE_STATIC_INIT(savePath)
#  define XX_PROFILE_STATIC_UNINIT()
#  define XX_PROFILE_ENABLED_ONLY(x)

#endif//XX_ENABLE_PROFILE

#endif//xxprofile_hpp
