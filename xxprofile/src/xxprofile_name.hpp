//
//  xxprofile_name.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/28.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_name_hpp
#define xxprofile_name_hpp
#include "xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct SName {
private:
    uint32_t id;
    
public:
    SName(const char* name);
    SName(uint32_t id);
    const char* c_str() const;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_name_hpp
