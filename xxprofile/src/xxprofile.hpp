//
//  xxprofile.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_hpp
#define xxprofile_hpp
#include "xxprofile_name.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class XXProfile;
class XXProfileTreeNode;

class XXProfile {
public:
    static bool StaticInit();
    static void StaticUninit();
    static bool IncreaseFrame();
};

class XXProfileScope {
public:
    XXProfileScope(const SName name);
    ~XXProfileScope();
    
private:
    XXProfileTreeNode* _node;
    XXProfile* _profile;

private:
    XX_CLASS_DELETE_COPY_AND_MOVE(XXProfileScope);
};

XX_NAMESPACE_END(xxprofile);

#if defined(_DEBUG) || defined(DEBUG)
#  ifndef XX_ENABLE_PROFILE
#    define XX_ENABLE_PROFILE 1
#  endif//XX_ENABLE_PROFILE
#endif//defined(_DEBUG) || defined(DEBUG)

#ifndef XX_ENABLE_PROFILE
#  define XX_ENABLE_PROFILE 0
#endif//XX_ENABLE_PROFILE

#if XX_ENABLE_PROFILE

#define HAVE_PRETTY_FUNCTION 1

#if HAVE_PRETTY_FUNCTION
#  define XX_PROFILE_FUNCTION __PRETTY_FUNCTION__
#endif//HAVE_PRETTY_FUNCTION

#ifndef XX_PROFILE_FUNCTION
#  define XX_PROFILE_FUNCTION __FUNCTION__
#endif//XX_PROFILE_FUNCTION

#  define XX_PROFILE_SCOPE_FUNCTION() \
/**/static ::xxprofile::SName __xxprofile_name_function_name(XX_PROFILE_FUNCTION); \
/**/::xxprofile::XXProfileScope __xxprofile_scope(__xxprofile_name_function_name); \

#  define XX_PROFILE_SCOPE_NAME(name) \
/**/static ::xxprofile::SName __xxprofile_name_##name##_name(#name); \
/**/::xxprofile::XXProfileScope __xxprofile_scope_##name(__xxprofile_name_##name##_name); \

#  define XX_PROFILE_INCREASE_FRAME() \
/**/::xxprofile::XXProfile::IncreaseFrame(); \

#  define XX_PROFILE_STATIC_INIT() \
/**/::xxprofile::XXProfile::StaticInit(); \

#  define XX_PROFILE_STATIC_UNINIT() \
/**/::xxprofile::XXProfile::StaticUninit(); \

#else//XX_ENABLE_PROFILE

#  define XX_PROFILE_SCOPE_FUNCTION()
#  define XX_PROFILE_SCOPE_NAME(name)
#  define XX_PROFILE_INCREASE_FRAME()
#  define XX_PROFILE_STATIC_INIT()
#  define XX_PROFILE_STATIC_UNINIT()

#endif//XX_ENABLE_PROFILE

#endif//xxprofile_hpp
