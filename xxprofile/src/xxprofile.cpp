//
//  xxprofile.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//

#include <stdio.h>
#include "xxprofile.hpp"
#include "platforms/platform.hpp"
#include <vector>

XX_NAMESPACE_BEGIN(xxprofile);

class XXProfileFunctionTreeNode {
    //std::vector<std::string, XXProfileFunctionTreeNode*> nodes;
};

class XXProfileThreadProfile {
    uint32_t _frameIndex;
    
    void beginFrame() {
        
    }
    
    void nextFrame() {
    }
};


//__thread XXProfileThreadProfile* g_thread_data = NULL;

XXProfileScope::XXProfileScope(const char* function) {
    
}

XXProfileScope::~XXProfileScope() {
    
}

XX_NAMESPACE_END(xxprofile);
