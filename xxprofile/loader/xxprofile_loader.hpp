// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_loader_xxprofile_loader_hpp
#define xxprofile_loader_xxprofile_loader_hpp
#include "../src/xxprofile_name.hpp"
#include "../src/xxprofile_namepool.hpp"
#include "../src/xxprofile_archive.hpp"
#include "../src/xxprofile_data.hpp"
#include <shared/utils/StrBuf.h>

XX_NAMESPACE_BEGIN(xxprofile);

struct XXProfileTreeNode;
struct Loader;

struct CombinedTreeItem;
struct TreeItem {
    const char* _name;
    const XXProfileTreeNode* _node;
    std::vector<TreeItem*>* _children;
    uint32_t _hash;
    // for combined
    TreeItem* _combinedNext;
    CombinedTreeItem* _combined;
    uint64_t _childrenCycles;

    void addChild(TreeItem* child) {
        if (!_children) {
            _children = new std::vector<TreeItem*>();
        }
        _children->push_back(child);
        _childrenCycles += child->useCycles();
    }

    uint32_t hash() const;

    bool same(const TreeItem& other) const;
    void debugDump(int indent) const;

    uint64_t useCycles() const;
    uint64_t childrenCycles() const {
        return _childrenCycles;
    }
};

struct FrameDataBase {
protected:
    uint64_t _frameCycles;
    uint32_t _frameId;
    uint32_t _nodeCount;
    XXProfileTreeNode* _nodes;
    TreeItem* _allNodes;

public:
    FrameDataBase() {
        memset(this, 0, sizeof(FrameDataBase));
    }
    FrameDataBase(FrameDataBase&& other) {
        memcpy(this, &other, sizeof(FrameDataBase));
        memset(&other, 0, sizeof(FrameDataBase));
    }
    ~FrameDataBase() {
        if (_allNodes) {
            for (uint32_t i = 0; i < _nodeCount; ++i) {
                auto& node = _allNodes[i];
                if (node._children) {
                    delete node._children;
                }
            }
            free(_allNodes);
        }
        if (_nodes) {
            free(_nodes);
        }
    }

    uint64_t frameCycles() const {
        return _frameCycles;
    }
    uint32_t frameId() const {
        return _frameId;
    }
    uint32_t nodeCount() const {
        return _nodeCount;
    }

private:
    XX_CLASS_DELETE_COPY(FrameDataBase);
    XX_CLASS_DELETE_MOVE_ASSIGN(FrameDataBase);
    friend Loader;
};

struct CombinedTreeItem {
    TreeItem* _firstItem;
    TreeItem* _lastItem;
    uint64_t _combinedCycles;
    uint64_t _combinedChildrenCycles;
    size_t _combinedCount;
    std::vector<CombinedTreeItem*>* _children;

    bool addChild(CombinedTreeItem* child, TreeItem* item);
    void combin(TreeItem* item);
    size_t combinedCount() const {
        return _combinedCount;
    }
    bool canCombin(TreeItem* item) const {
        assert(_firstItem);
        return _firstItem && _firstItem->same(*item);
    }
    uint64_t useCycles() const {
        return _combinedCycles;
    }
    uint64_t childrenCycles() const {
        return _combinedChildrenCycles;
    }
    const char* name() const {
        assert(_firstItem);
        return _firstItem->_name;
    }

    void sortChildren() {
        if (_children) {
            std::sort(_children->begin(), _children->end(), CombinedTreeItem::compare);
        }
    }

    static bool compare(const CombinedTreeItem* a, const CombinedTreeItem* b) {
        return b->_combinedCycles < a->_combinedCycles;
    }

    void calcCombinedChildrenCycles() {
        if (_children) {
            _combinedChildrenCycles = 0;
            for (auto i = _children->begin(), end = _children->end(); i != end; ++i) {
                _combinedChildrenCycles += (*i)->_combinedCycles;
            }
        }
    }
};

struct FrameData : FrameDataBase {
protected:
    std::vector<TreeItem*> _roots;
    uint32_t _combinedNodeCount;
    CombinedTreeItem* _allCombinedNodes;
    std::vector<CombinedTreeItem*> _combinedRoots;

public:
    FrameData() : _combinedNodeCount(0), _allCombinedNodes(NULL) {
    }

    FrameData(FrameData&& other) : FrameDataBase(std::move(other)), _roots(std::move(other._roots)), _combinedRoots(std::move(other._combinedRoots)) {
        _combinedNodeCount = other._combinedNodeCount;
        other._combinedNodeCount = 0;
        _allCombinedNodes = other._allCombinedNodes;
        other._allCombinedNodes = NULL;
    }

    ~FrameData() {
        if (_allCombinedNodes) {
            for (uint32_t i = 0; i < _nodeCount; ++i) {
                auto& node = _allCombinedNodes[i];
                if (node._children) {
                    delete node._children;
                }
            }
            free(_allCombinedNodes);
        }
    }

    void init(Loader* loader);

    const std::vector<TreeItem*>& roots() const {
        return _roots;
    }

    const std::vector<CombinedTreeItem*>& combinedRoots() const {
        return _combinedRoots;
    }

    bool anyCombined() const {
        return _combinedNodeCount != _nodeCount;
    }

    uint64_t startTime() const {
        uint64_t time = _roots[0]->_node->_beginTime;
        for (auto i = _roots.begin() + 1; i < _roots.end(); ++i) {
            if (time > (*i)->_node->_beginTime) {
                time = (*i)->_node->_beginTime;
            }
        }
        return time;
    }

    uint64_t endTime() const {
        uint64_t time = _roots[0]->_node->_endTime;
        for (auto i = _roots.begin() + 1; i < _roots.end(); ++i) {
            if (time < (*i)->_node->_endTime) {
                time = (*i)->_node->_endTime;
            }
        }
        return time;
    }

private:
    XX_CLASS_DELETE_COPY(FrameData);
    XX_CLASS_DELETE_MOVE_ASSIGN(FrameData);
};

struct ThreadData {
    std::vector<FrameData> _frames;
    uint32_t _threadId;
    uint64_t _maxCycleCount;
    double _secondsPerCycle;
    ThreadData() {
        _threadId = 0;
        _maxCycleCount = 0;
        _secondsPerCycle = 0;
    }

    ~ThreadData();
    void clear();

    ThreadData(ThreadData&& other) : _frames(std::move(other._frames)) {
        _threadId = other._threadId;
        _maxCycleCount = other._maxCycleCount;
        _secondsPerCycle = other._secondsPerCycle;
    }

    void swap(ThreadData& other) {
        _frames.swap(other._frames);
        std::swap(_threadId, other._threadId);
        std::swap(_maxCycleCount, other._maxCycleCount);
        std::swap(_secondsPerCycle, other._secondsPerCycle);
    }

private:
    XX_CLASS_DELETE_COPY(ThreadData);
    XX_CLASS_DELETE_MOVE_ASSIGN(ThreadData);
};

// uint32_t threadId; // from version 3
// uint32_t frameId;
// SName::Serialize();
// uint32_t nodeCount;
// XXProfileTreeNode nodes[nodeCount];
struct Loader {
    std::vector<ThreadData> _threads;
    SNamePool _namePool;
    std::vector<const char*> _names;
    double _secondsPerCycle;
    uint64_t _processStart;

    Loader();
    ~Loader();

    void load(Archive& ar);
    void clear();

    const char* name(SName name);

protected:
    ThreadData& getThreadFromId(uint32_t threadId);
    static const char* prepareName(const char* name);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_loader_xxprofile_loader_hpp
