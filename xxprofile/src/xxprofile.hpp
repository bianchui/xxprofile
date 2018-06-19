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

class XXProfileFunctionTreeNode;

class XXProfileScope {
public:
    XXProfileScope(const char* function);
    ~XXProfileScope();
    
private:
    XXProfileFunctionTreeNode* _node;
};

XX_NAMESPACE_END(xxprofile);

#if defined(_DEBUG)
#  ifndef XX_ENABLE_PROFILE
#    define XX_ENABLE_PROFILE 1
#  endif//XX_ENABLE_PROFILE
#endif//_DEBUG

#endif//xxprofile_hpp
