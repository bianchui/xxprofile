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

class XXProfileFunctionTreeNode;

class XXProfileScope {
public:
    XXProfileScope(const char* function);
    ~XXProfileScope();
    
private:
    XXProfileFunctionTreeNode* _node;
};

#if defined(_DEBUG) || defined(XX_ENABLE_PROFILE)

#define XX_PROFILE_

#endif//_DEBUG

#endif//xxprofile_hpp
