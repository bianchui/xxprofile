#include "FrameView.hpp"
#include "imgui/imgui_custom.hpp"
#include "../src/xxprofile_tls.hpp"

FrameView::FrameView() : _loader(NULL), _frameData(NULL), _allNodes(NULL) {
    
}

FrameView::~FrameView() {
    clear();
}

void FrameView::clear() {
    _roots.clear();
    if (_allNodes) {
        for (uint32_t i = 0; i < _frameData->nodeCount; ++i) {
            auto& node = _allNodes[i];
            if (node.children) {
                delete node.children;
            }
        }
        free(_allNodes);
        _allNodes = NULL;
    }
    _frameData = NULL;
}

void FrameView::setLoader(const xxprofile::Loader* loader) {
    _loader = loader;
}

void FrameView::setFrameData(const xxprofile::Loader::FrameData* data) {
    clear();
    _frameData = data;
    if (data && data->nodeCount) {
        assert(_loader);
        _allNodes = (TreeItem*)malloc(sizeof(TreeItem) * data->nodeCount);
        memset(_allNodes, 0, sizeof(TreeItem) * data->nodeCount);
        const xxprofile::XXProfileTreeNode* nodes = data->nodes;
        const uint32_t nodeCount = data->nodeCount;
        for (uint32_t i = 0; i < nodeCount; ++i) {
            const xxprofile::XXProfileTreeNode* node = nodes + i;
            TreeItem* item = _allNodes + i;
            item->node = node;
            item->name = _loader->name(node->_name);
            if (node->_parentNodeId) {
                assert(node->_parentNodeId < i);
                TreeItem* parentItem = _allNodes + (node->_parentNodeId - 1);
                parentItem->addChild(item);
            } else {
                _roots.push_back(item);
            }
        }
    }
}

void FrameView::draw() {
    struct DrawTreeNode {
        static void Draw(TreeItem* item) {
            if (item->children) {
                if (ImGui::TreeNode(item->name)) {
                    for (auto iter = item->children->begin(); iter != item->children->end(); ++iter) {
                        Draw(*iter);
                    }
                    ImGui::TreePop();
                }
            } else {
                ImGui::BulletText("%s", item->name);
            }
        }
    };
    DrawTreeNode drawtv;
    for (auto iter = _roots.begin(); iter != _roots.end(); ++iter) {
        drawtv.Draw(*iter);
    }
}
