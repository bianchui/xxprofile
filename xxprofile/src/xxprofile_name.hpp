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

class Archive;

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

public:
    struct IncrementSerializeTag {
        uint32_t fromId;
    };
    static void Serialize(IncrementSerializeTag* tag, Archive& ar);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_name_hpp
