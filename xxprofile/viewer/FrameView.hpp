// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_FrameView_hpp
#define xxprofileviewer_FrameView_hpp
#include "../loader/xxprofile_loader.hpp"

class FrameView {
public:
    FrameView();
    ~FrameView();

    void setLoader(const xxprofile::Loader* loader);
    void setFrameData(const xxprofile::Loader::FrameData* data);
    void draw();

    void clear();

    struct TreeItem {
        const char* name;
        const xxprofile::XXProfileTreeNode* node;
        std::vector<TreeItem*>* children;
        bool open;

        void addChild(TreeItem* child) {
            if (!children) {
                children = new std::vector<TreeItem*>();
            }
            children->push_back(child);
        }
    };

private:
    const xxprofile::Loader* _loader;
    const xxprofile::Loader::FrameData* _frameData;
    TreeItem* _allNodes;
    std::vector<TreeItem*> _roots;
};

#endif//xxprofileviewer_FrameView_hpp
