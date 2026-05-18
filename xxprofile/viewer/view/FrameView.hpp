// Copyright (C) 2018-2025, bianchui. All rights reserved.
#ifndef xxprofileviewer_FrameView_hpp
#define xxprofileviewer_FrameView_hpp
#include "../../loader/xxprofile_loader.hpp"

class FrameView {
public:
    FrameView();
    ~FrameView();

    void setLoader(const xxprofile::Loader* loader);
    void setFrameData(const xxprofile::FrameData* data);
    void setFrameNodeData(const xxprofile::FrameData* data, const xxprofile::XXProfileTreeNode* node);
    void draw();

    void clear();
    void setCombined(bool combined) {
        _combined = combined;
    }

private:
    const xxprofile::Loader* _loader;
    const xxprofile::FrameData* _frameData;
    const xxprofile::FrameDetail* _frameDetail;
    const xxprofile::XXProfileTreeNode* _focusNode;
    bool _focusNodePending;
    bool _combined;
    uint64_t _frameStart;
};

#endif//xxprofileviewer_FrameView_hpp
