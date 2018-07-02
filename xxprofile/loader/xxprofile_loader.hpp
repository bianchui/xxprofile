// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_loader_xxprofile_loader_hpp
#define xxprofile_loader_xxprofile_loader_hpp
#include "../src/xxprofile_name.hpp"
#include "../src/xxprofile_namepool.hpp"
#include "../src/xxprofile_archive.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct XXProfileTreeNode;

// uint32_t frameId;
// SName::Serialize();
// uint32_t nodeCount;
// XXProfileTreeNode nodes[nodeCount];
struct Loader {
    struct FrameData {
        uint32_t frameId;
        uint32_t nodeCount;
        XXProfileTreeNode* nodes;
    };

    struct ThreadData {
        std::vector<FrameData> _frames;
        double _secondsPerCycle;
        ThreadData() {
        }

        ~ThreadData();
        void clear();

        ThreadData(ThreadData&& other) : _frames(std::move(other._frames)) {
        }

        void swap(ThreadData& other) {
            _frames.swap(other._frames);
        }

    private:
        XX_CLASS_DELETE_COPY(ThreadData);
        XX_CLASS_DELETE_MOVE_ASSIGN(ThreadData);
    };

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
