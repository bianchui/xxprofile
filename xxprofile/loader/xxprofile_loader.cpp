// Copyright 2017 bianchui. All rights reserved.
#include "../src/xxprofile_internal.hpp"
#include "xxprofile_loader.hpp"
#include "../src/xxprofile_tls.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

Loader::ThreadData::~ThreadData() {
    clear();
}

void Loader::ThreadData::clear() {
    for (auto iter = _frames.begin(); iter != _frames.end(); ++iter) {
        if (iter->nodes) {
            free(iter->nodes);
        }
    }
    _frames.clear();
}

Loader::Loader() {
}

Loader::~Loader() {
    clear();
}

void Loader::load(Archive& ar) {
    ThreadData thread;
    while (!ar.eof()) {
        FrameData data;
        memset(&data, 0, sizeof(FrameData));
        ar << data.frameId;
        _namePool.serialize(NULL, ar);
        ar << data.nodeCount;
        if (data.nodeCount > 0) {
            data.nodes = (XXProfileTreeNode*)malloc(sizeof(XXProfileTreeNode) * data.nodeCount);
            ar.serialize(data.nodes, sizeof(XXProfileTreeNode) * data.nodeCount);
        }
        thread._frames.push_back(data);
    }
    _threads.push_back(std::move(thread));
}

void Loader::clear() {
    _threads.clear();
    _namePool.clear();
}

XX_NAMESPACE_END(xxprofile);
