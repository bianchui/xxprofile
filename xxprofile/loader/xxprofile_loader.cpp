// Copyright 2017 bianchui. All rights reserved.
#include "../src/xxprofile_internal.hpp"
#include "xxprofile_loader.hpp"
#include "../src/xxprofile_tls.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

uint64_t TreeItem::useCycles() const {
    return node->_endTime - node->_beginTime;
}

void FrameData::init(Loader* loader) {
    assert(loader);
    assert(_allNodes == NULL);
    if (_nodeCount) {
        assert(_nodes != NULL);
        assert(_frameCycles == 0);
        _allNodes = (TreeItem*)malloc(sizeof(TreeItem) * _nodeCount);
        memset(_allNodes, 0, sizeof(TreeItem) * _nodeCount);
        const xxprofile::XXProfileTreeNode* nodes = _nodes;
        const uint32_t nodeCount = _nodeCount;

        _frameCycles = 0;
        for (uint32_t i = 0; i < nodeCount; ++i) {
            const xxprofile::XXProfileTreeNode* node = nodes + i;
            TreeItem* item = _allNodes + i;
            item->node = node;
            item->name = loader->name(node->_name);
            if (node->_parentNodeId) {
                assert(node->_parentNodeId <= i);
                TreeItem* parentItem = _allNodes + (node->_parentNodeId - 1);
                parentItem->addChild(item);
            } else {
                _roots.push_back(item);
                _frameCycles += item->useCycles();
            }
        }
    }
}

ThreadData::~ThreadData() {
    clear();
}

void ThreadData::clear() {
    _frames.clear();
    _secondsPerCycle = 0;
    _maxCycleCount = 0;
}

Loader::Loader() {
}

Loader::~Loader() {
    clear();
}

void Loader::load(Archive& ar) {
    ThreadData thread;
    ar << thread._secondsPerCycle;
    thread._maxCycleCount = 0;
    while (!ar.eof()) {
        FrameData data;
        ar << data._frameId;
        XXLOG_DEBUG("Load.frame(%d)\n", data._frameId);
        _namePool.serialize(NULL, ar);
        ar << data._nodeCount;
        XXLOG_DEBUG("  nodeCount = %d\n", data._nodeCount);
        if (data._nodeCount > 0) {
            data._nodes = (XXProfileTreeNode*)malloc(sizeof(XXProfileTreeNode) * data._nodeCount);
            ar.serialize(data._nodes, sizeof(XXProfileTreeNode) * data._nodeCount);
        }
        data.init(this);
        if (thread._maxCycleCount < data.frameCycles()) {
            thread._maxCycleCount = data.frameCycles();
        }
        thread._frames.push_back(std::move(data));
    }
    _threads.push_back(std::move(thread));
}

void Loader::clear() {
    _threads.clear();
    _namePool.clear();
    for (auto iter = _names.begin(); iter != _names.end(); ++iter) {
        if (*iter) {
            free((void*)*iter);
        }
    }
    _names.clear();
}

const char* Loader::name(SName name) {
    if (name.id() == 0) {
        return "";
    }
    uint32_t index = name.id() - 1;
    const char* n = NULL;
    if (index < _names.size()) {
        n = _names[index];
    } else {
        _names.resize(index + 1);
    }
    if (n) {
        return n;
    }
    _names[index] = n = prepareName(_namePool.getName(name));
    return n;
}

const char* Loader::prepareName(const char* name) {
    return strdup(name);
}

XX_NAMESPACE_END(xxprofile);
