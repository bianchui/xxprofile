#ifndef xxprofile_data_hpp
#define xxprofile_data_hpp
#include "xxprofile_name.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

// XXProfileTreeNode
struct XXProfileTreeNode {
    uint64_t _beginTime;
    uint64_t _endTime;
    SName _name;
    //uint32_t _nodeId;
    uint32_t _parentNodeId;
};

static_assert(sizeof(XXProfileTreeNode) == (8+8+4+4), "NodeSize");

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_data_hpp
