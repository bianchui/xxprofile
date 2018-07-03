// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_loader_xxprofile_loader_hpp
#define xxprofile_loader_xxprofile_loader_hpp
#include "../src/xxprofile_name.hpp"
#include "../src/xxprofile_namepool.hpp"
#include "../src/xxprofile_archive.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct XXProfileTreeNode;
struct Loader;

struct TreeItem {
    const char* name;
    const XXProfileTreeNode* node;
    std::vector<TreeItem*>* children;

    void addChild(TreeItem* child) {
        if (!children) {
            children = new std::vector<TreeItem*>();
        }
        children->push_back(child);
    }

    uint64_t useCycles() const;
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
                if (node.children) {
                    delete node.children;
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

struct FrameData : FrameDataBase {
protected:
    std::vector<TreeItem*> _roots;

public:
    FrameData() {
    }

    FrameData(FrameData&& other) : FrameDataBase(std::move(other)), _roots(std::move(other._roots)) {
    }

    ~FrameData() {
    }

    void init(Loader* loader);

    const std::vector<TreeItem*>& roots() const {
        return _roots;
    }
private:
    XX_CLASS_DELETE_COPY(FrameData);
    XX_CLASS_DELETE_MOVE_ASSIGN(FrameData);
};

struct ThreadData {
    std::vector<FrameData> _frames;
    uint64_t _maxCycleCount;
    double _secondsPerCycle;
    ThreadData() {
        _maxCycleCount = 0;
        _secondsPerCycle = 0;
    }

    ~ThreadData();
    void clear();

    ThreadData(ThreadData&& other) : _frames(std::move(other._frames)) {
        _maxCycleCount = other._maxCycleCount;
        _secondsPerCycle = other._secondsPerCycle;
    }

    void swap(ThreadData& other) {
        _frames.swap(other._frames);
        std::swap(_maxCycleCount, other._maxCycleCount);
        std::swap(_secondsPerCycle, other._secondsPerCycle);
    }

private:
    XX_CLASS_DELETE_COPY(ThreadData);
    XX_CLASS_DELETE_MOVE_ASSIGN(ThreadData);
};

// uint32_t frameId;
// SName::Serialize();
// uint32_t nodeCount;
// XXProfileTreeNode nodes[nodeCount];
struct Loader {
    std::vector<ThreadData> _threads;
    SNamePool _namePool;

    Loader();
    ~Loader();

    void load(Archive& ar);
    void clear();

    FORCEINLINE const char* name(SName name) const {
        return _namePool.getName(name);
    }
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_loader_xxprofile_loader_hpp
