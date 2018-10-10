// Copyright 2017 bianchui. All rights reserved.
#include "../src/xxprofile_internal.hpp"
#include "xxprofile_loader.hpp"
#include "xxprofile_CppNameDecoder.hpp"
#include <zlib.h>

XX_NAMESPACE_BEGIN(xxprofile);

FORCEINLINE uint32_t Uint32Hash(uint32_t value, uint32_t hash = 0) {
    // BKDR Hash Function
    uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    return hash * seed + value;
    union B4 {
        char ch[4];
        uint32_t ui;
    };
    B4 b;
    b.ui = value;
    bool have = false;
    for (uint32_t i = 0; i < 4; ++i) {
        if (!have && !b.ch[3-i]) {
            continue;
        }
        have = true;
        hash = hash * seed + b.ch[3-i];
    }
    return hash;
}

//----------
// TreeItem
uint32_t TreeItem::hash() const {
    if (!_hash) {
        uint32_t h = Uint32Hash(_node->_name.id(), 0);

        if (_children) {
            for (uint32_t i = 0; i < _children->size(); ++i) {
                h = Uint32Hash((*_children)[i]->hash(), h);
            }
        }
        const_cast<uint32_t&>(_hash) = h;
    }
    return _hash;
}

bool TreeItem::same(const TreeItem& other) const {
    if (hash() != other.hash()) {
        return false;
    }
    if (_node->_name.id() != other._node->_name.id()) {
        return false;
    }
    const size_t c0 = _children ? _children->size() : 0;
    const size_t c1 = other._children ? other._children->size() : 0;
    if (c0 != c1) {
        return false;
    }
    if (!c0) {
        return true;
    }
    for (size_t i = 0; i < c0; ++i) {
        if (!(*_children)[i]->same(*(*other._children)[i])) {
            return false;
        }
    }
    return true;
}

uint64_t TreeItem::useCycles() const {
    return _node->_endTime - _node->_beginTime;
}

//----------
// CombinedTreeItem
bool CombinedTreeItem::addChild(CombinedTreeItem* child, TreeItem* item) {
    if (!_children) {
        _children = new std::vector<CombinedTreeItem*>();
        child->combin(item);
        _children->push_back(child);
        return true;
    }
    const auto end = _children->end();
    for (auto iter = _children->begin(); iter != end; ++iter) {
        if ((*iter)->canCombin(item)) {
            (*iter)->combin(item);
            return false;
        }
    }
    child->combin(item);
    _children->push_back(child);
    return true;
}

void CombinedTreeItem::combin(TreeItem* item) {
    assert(item->_combined == NULL);
    assert(item->_combinedNext == NULL);
    if (_firstItem) {
        assert(_combinedCount);
        assert(_lastItem);
        _lastItem->_combinedNext = item;
    } else {
        assert(_combinedCount == 0);
        assert(_lastItem == NULL);
        _firstItem = _lastItem = item;
    }
    item->_combined = this;
    item->_combinedNext = _firstItem;
    _combinedTime += item->_node->_endTime - item->_node->_beginTime;
    ++_combinedCount;
}

//----------
// FrameData
void FrameData::init(Loader* loader) {
    assert(loader);
    assert(_allNodes == NULL);
    _combinedNodeCount = NULL;
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
            item->_node = node;
            item->_name = loader->name(node->_name);
            if (node->_parentNodeId) {
                assert(node->_parentNodeId <= i);
                if (node->_parentNodeId > i) {
                    continue;
                }
                TreeItem* parentItem = _allNodes + (node->_parentNodeId - 1);
                parentItem->addChild(item);
            } else {
                _roots.push_back(item);
                _frameCycles += item->useCycles();
            }
        }

        // combin function calls
        _allCombinedNodes = (CombinedTreeItem*)malloc(sizeof(CombinedTreeItem) * _nodeCount);
        memset(_allCombinedNodes, 0, sizeof(CombinedTreeItem) * _nodeCount);
        for (uint32_t i = 0; i < nodeCount; ++i) {
            TreeItem* item = _allNodes + i;
            CombinedTreeItem* combined = _allCombinedNodes + i;
            assert(item->_combined == NULL);
            if (item->_node->_parentNodeId) {
                assert(item->_node->_parentNodeId <= i);
                if (item->_node->_parentNodeId > i) {
                    continue;
                }
                TreeItem* parentItem = _allNodes + (item->_node->_parentNodeId - 1);
                assert(parentItem->_combined);
                if (parentItem->_combined->addChild(combined, item)) {
                    ++_combinedNodeCount;
                }
            } else {
                bool isCombined = false;
                for (auto iter = _combinedRoots.begin(); iter != _combinedRoots.end(); ++iter) {
                    if ((*iter)->canCombin(item)) {
                        (*iter)->combin(item);
                        isCombined = true;
                        break;
                    }
                }
                if (!isCombined) {
                    combined->combin(item);
                    _combinedRoots.push_back(combined);
                    ++_combinedNodeCount;
                }
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
    while (!ar.eof() && !ar.hasError()) {
        FrameData data;
        ar << data._frameId;
        XXLOG_DEBUG("Load.frame(%d)\n", data._frameId);
        _namePool.serialize(NULL, ar);
        ar << data._nodeCount;
        XXLOG_DEBUG("  nodeCount = %d\n", data._nodeCount);
        if (!ar.hasError() && data._nodeCount > 0) {
            size_t remainSize = sizeof(XXProfileTreeNode) * data._nodeCount;
            data._nodes = (XXProfileTreeNode*)malloc(remainSize);
            memset(data._nodes, 0, remainSize);
            if (ar.version() == 1) {
                ar.serialize(data._nodes, remainSize);
            } else if (ar.version() == 2) {
                bool hasError = false;
                Bytef* buf = NULL;
                size_t bufSize = 0;
                Bytef* cur = (Bytef*)data._nodes;
                while (!ar.hasError()) {
                    uint32_t sizeOrg = 0, sizeCom = 0;
                    ar << sizeOrg;
                    ar << sizeCom;
                    if (ar.hasError()) {
                        break;
                    }
                    if (remainSize < sizeOrg) {
                        hasError = true;
                        break;
                    }
                    if (sizeCom) {
                        if (bufSize) {
                            if (bufSize < sizeOrg) {
                                hasError = true;
                                break;
                            }
                        } else {
                            bufSize = sizeOrg;
                            buf = (Bytef*)malloc(bufSize);
                        }
                        ar.serialize(buf, sizeCom);
                        if (ar.hasError()) {
                            break;
                        }
                        uLong destLen = sizeOrg;
                        if (Z_OK != uncompress(cur, &destLen, buf, sizeCom) || destLen != sizeOrg) {
                            hasError = true;
                            break;
                        }
                    } else {
                        ar.serialize(cur, sizeOrg);
                    }
                    cur += sizeOrg;
                    remainSize -= sizeOrg;
                    if (remainSize == 0) {
                        break;
                    }
                }
                if (buf) {
                    free(buf);
                }
                if (hasError || remainSize != 0) {
                    break;
                }
            }
        }
        if (ar.hasError()) {
            break;
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
    //CppNameDecoder decoder(name);

    return strdup(name);
}

XX_NAMESPACE_END(xxprofile);
