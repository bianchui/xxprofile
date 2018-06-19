//
//  xxprofile.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_hpp
#define xxprofile_hpp
#include "platforms/platform.hpp"
#include "xxprofile_name.hpp"
#include <vector>

XX_NAMESPACE_BEGIN(xxprofile);

class XXProfile;
class XXProfileTreeNode;

class XXProfile {
public:
    enum {
        ChunkNodeCount = 256 * 1024,
    };

public:
    static bool StaticInit();
    static XXProfile* Get();
    static void IncreaseFrame();

    XXProfile();

    void increaseFrame();
    XXProfileTreeNode* beginScope(SName name);
    void endScope(XXProfileTreeNode* node);

protected:
    XXProfileTreeNode* newChunk();

private:
    uint64_t _frameId;
    std::vector<XXProfileTreeNode*> _stack;
    size_t _usedCount;
    uint32_t _curNodeId;

    // allocation
private:
    std::vector<XXProfileTreeNode*> _buffers;
    std::vector<XXProfileTreeNode*> _freeBuffers;
    XXProfileTreeNode* _currentBuffer;
};

class XXProfileScope {
public:
    XXProfileScope(const SName name) {
        _profile = XXProfile::Get();
        _node = _profile->beginScope(name);
    }

    ~XXProfileScope() {
        _profile->endScope(_node);
    }

private:
    XXProfileTreeNode* _node;
    XXProfile* _profile;
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

#  define XX_PROFILE_SCOPE_FUNCTION() \
/**/static ::xxprofile::SName __xxprofile_name_function_name(__FUNCTION__); \
/**/::xxprofile::XXProfileScope __xxprofile_scope(__xxprofile_name_function_name); \

#  define XX_PROFILE_INCREASE_FRAME() \
/**/::xxprofile::XXProfile::IncreaseFrame(); \

#else//XX_ENABLE_PROFILE

#  define XX_PROFILE_SCOPE_FUNCTION()
#  define XX_PROFILE_INCREASE_FRAME()

#endif//XX_ENABLE_PROFILE

#endif//xxprofile_hpp
