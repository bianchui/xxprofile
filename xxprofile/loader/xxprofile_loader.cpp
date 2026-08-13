// Copyright (C) 2017-2025, bianchui. All rights reserved.
#include "../src/xxprofile_internal.hpp"
#include "../src/xxprofile_version.hpp"
#include "xxprofile_loader.hpp"
#include "xxprofile_CppNameDecoder.hpp"
#include "../src/compress/compress_zlib.cpp.h"
#include "../src/compress/compress_lzo.cpp.h"
#include "../src/compress/compress_lz4.cpp.h"
#include "../src/compress/compress_zstd.cpp.h"

#define TEST_COMPRESS 0

XX_NAMESPACE_BEGIN(xxprofile);

struct SDecompress {
    uint32_t _method;
    IDecompress* _decompress;

    SDecompress(uint32_t method) {
        _method = method;
        switch (_method) {
            case ECompressMethod::Zlib:
                _decompress = new SDecompressZlib();
                break;
            case ECompressMethod::Lzo:
                _decompress = new SDecompressLzo();
                break;
            case ECompressMethod::Lz4:
                _decompress = new SDecompressLz4();
                break;
            case ECompressMethod::Zstd:
                _decompress = new SDecompressZstd();
                break;
            case ECompressMethod::ZlibChunked:
                _decompress = new SDecompressChunkedZlib();
                break;
            case ECompressMethod::LzoChunked:
                //_decompress = new SDecompressChunkedLzo();
                assert(false);
                break;
            case ECompressMethod::Lz4Chunked:
                _decompress = new SDecompressChunkedLz4();
                break;
            case ECompressMethod::ZstdChunked:
                _decompress = new SDecompressChunkedZstd();
                break;
            default:
                assert(false);
                break;
        }
    }

    size_t operator ()(void* dst, size_t dstSize, const void* src, size_t srcSize) {
        return _decompress->doDecompress(dst, dstSize, src, srcSize);
    }
};

FORCEINLINE uint32_t Uint32Hash(uint32_t value, uint32_t hash = 0) {
    // BKDR Hash Function
    uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    return hash * seed + value;
#if 0
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
#endif//0
}

#pragma mark - FrameData

static std::vector<uint32_t> g_depth;
static const double kFlushHideGapSeconds = 0.1;

bool FrameData::init(const Loader& loader) {
    const uint32_t name_xxflush = loader.findNameId("xxflush");
    _nodeCountWithoutFlush = _nodeCount;
    if (_nodeCount) {
        const uint32_t nodeCount = _nodeCount;
        assert(_nodes != nullptr);
        assert(_frameCycles == 0);
        const xxprofile::XXProfileTreeNode* nodes = _nodes;

        g_depth.resize(nodeCount);
        memset(g_depth.data(), 0, nodeCount * sizeof(uint32_t));

        uint32_t maxDepth = 0;
        for (uint32_t i = 0; i < nodeCount; ++i) {
            const xxprofile::XXProfileTreeNode* node = nodes + i;
            if (!node->_parentNodeId) {
                _frameCycles += node->_endTime - node->_beginTime;
                if (i == 0) {
                    _startTime = node->_beginTime;
                    _endTime = node->_endTime;
                } else {
                    if (_startTime > node->_beginTime) {
                        _startTime = node->_beginTime;
                    }
                    if (_endTime < node->_endTime) {
                        _endTime = node->_endTime;
                    }
                }
            } else if (node->_parentNodeId <= nodeCount) {
                const uint32_t parentIndex = node->_parentNodeId - 1;
                if (parentIndex >= i) {
                    return false;
                }
                const uint32_t depth = g_depth[i] = g_depth[parentIndex] + 1;
                if (maxDepth < depth) {
                    maxDepth = depth;
                }
            } else {
                return false;
            }
        }
        _maxCallDepth = maxDepth + 1;
        _endTimeWithoutFlush = _endTime;

        if (name_xxflush != (uint32_t)-1 && nodeCount > 1 && loader.secondsPerCycle() > 0) {
            const XXProfileTreeNode& trailingNode = nodes[nodeCount - 1];
            if (!trailingNode._parentNodeId && trailingNode._name.id() == name_xxflush) {
                uint64_t endTimeWithoutFlush = 0;
                for (uint32_t i = 0; i + 1 < nodeCount; ++i) {
                    const XXProfileTreeNode& node = nodes[i];
                    if (!node._parentNodeId) {
                        endTimeWithoutFlush = std::max(endTimeWithoutFlush, node._endTime);
                    }
                }
                if (endTimeWithoutFlush >= _startTime &&
                    trailingNode._beginTime > endTimeWithoutFlush &&
                    (trailingNode._beginTime - endTimeWithoutFlush) * loader.secondsPerCycle() > kFlushHideGapSeconds) {
                    _endTimeWithoutFlush = endTimeWithoutFlush;
                    _nodeCountWithoutFlush = nodeCount - 1;
                }
            }
        }
    }
    return true;
}

#pragma mark - TreeItem

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

void TreeItem::debugDump(int indent) const {
    printf("%*s%s\n", indent * 2, "", _name);
    if (_children) {
        for (size_t i = 0; i < _children->size(); ++i) {
            (*_children)[i]->debugDump(indent + 1);
        }
    }
}

uint64_t TreeItem::useCycles() const {
    return _node->_endTime - _node->_beginTime;
}

#pragma mark - CombinedTreeItem

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
    assert(item->_combined == nullptr);
    assert(item->_combinedNext == nullptr);
    if (_firstItem) {
        assert(_combinedCount);
        assert(_lastItem);
        _lastItem->_combinedNext = item;
    } else {
        assert(_combinedCount == 0);
        assert(_lastItem == nullptr);
        _firstItem = _lastItem = item;
    }
    item->_combined = this;
    item->_combinedNext = _firstItem;
    _combinedCycles += item->useCycles();
    ++_combinedCount;
}

#pragma mark - FrameDetail

FrameDetail::FrameDetail(const Loader& loader, const FrameData& data) {
    assert(_allNodes == nullptr);
    _combinedNodeCount = 0;
    _allCombinedNodes = nullptr;
    _frameId = data._frameId;
    const uint32_t nodeCount = _nodeCount = data._nodeCount;
    _nodes = data._nodes;
    if (nodeCount) {
        assert(_nodes != nullptr);
        assert(_frameCycles == 0);
        _allNodes = (TreeItem*)malloc(sizeof(TreeItem) * nodeCount);
        memset(_allNodes, 0, sizeof(TreeItem) * nodeCount);
        const xxprofile::XXProfileTreeNode* nodes = _nodes;

        _frameCycles = 0;
        for (uint32_t i = 0; i < nodeCount; ++i) {
            const xxprofile::XXProfileTreeNode* node = nodes + i;
            TreeItem* item = _allNodes + i;
            item->_node = node;
            item->_name = loader.name(node->_name);
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
        _allCombinedNodes = (CombinedTreeItem*)malloc(sizeof(CombinedTreeItem) * nodeCount);
        memset(_allCombinedNodes, 0, sizeof(CombinedTreeItem) * nodeCount);
        for (uint32_t i = 0; i < nodeCount; ++i) {
            TreeItem* item = _allNodes + i;
            CombinedTreeItem* combined = _allCombinedNodes + i;
            assert(item->_combined == nullptr);
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
        for (size_t i = 0; i < nodeCount; ++i) {
            _allCombinedNodes[i].sortChildren();
            _allCombinedNodes[i].calcCombinedChildrenCycles();
        }
    }
}

#pragma mark - ThreadData

ThreadData::~ThreadData() {
    clear();
}

void ThreadData::clear() {
    _frames.clear();
    _secondsPerCycle = 0;
    _maxCycleCount = 0;
}

static bool isLower(const FrameData& frame, uint64_t time) {
    return frame.endTime() < time;
}

uint32_t ThreadData::findFirstFrame(uint64_t startTime) const {
    const auto iter = std::lower_bound(_frames.begin(), _frames.end(), startTime, isLower);
    return static_cast<uint32_t>(iter - _frames.begin());
}

#pragma mark - Loader

Loader::Loader() {
}

Loader::~Loader() {
    clear();
}

ThreadData& Loader::getThreadFromId(uint32_t threadId) {
    for (auto iter = _threads.begin(), end = _threads.end(); iter != end; ++iter) {
        if (iter->_threadId == threadId) {
            return *iter;
        }
    }
    {
        size_t index = _threads.size();
        _threads.resize(index + 1);
        ThreadData& thread = _threads[index];
        thread._threadIndex = static_cast<uint32_t>(index);
        thread._threadId = threadId;
        thread._secondsPerCycle = this->_secondsPerCycle;
        thread._maxCycleCount = 0;
        return thread;
    }
}

struct Buffer {
    uint8_t* buf = nullptr;
    uint64_t bufSize = 0;

    ~Buffer() {
        if (buf) {
            free(buf);
        }
    }

    uint8_t* get(size_t size) {
        if (bufSize < size) {
            bufSize = size + 1024;
            buf = (uint8_t*)realloc(buf, bufSize);
        }
        return buf;
    }
};

#if TEST_COMPRESS
uint32_t compressSize(xxprofile::ICompress& compress, const void* data, uint32_t sizeData, Buffer& buf) {
    uint32_t size = (uint32_t)compress.calcBound(sizeData);
    return (uint32_t)compress.doCompress(buf.get(size), size, data, sizeData);
}
#endif//TEST_COMPRESS

void Loader::load(Archive& ar) {
    _processStart = 0;
    _processEnd = 0;
    _fileSize = 0;
    _dataSize = 0;
    SDecompress decompress(ar.getCompressMethod());
#if TEST_COMPRESS
    SCompressZstd compress;
    SCompressChunkedZstd compressChunked;
    Buffer compressBuf;
    uint64_t fileSizeTest = 0;
#endif//TEST_COMPRESS
    ar << this->_secondsPerCycle;
    uint32_t threadId = 0;
    Buffer buf;
    while (!ar.eof() && !ar.hasError()) {
        if (ar.version() >= EVersion::V3) {
            ar << threadId;
            if (ar.eof() || ar.hasError()) {
                break;
            }
        }
        ThreadData& thread = getThreadFromId(threadId);
        uint32_t threadNameId = -1;
        if (ar.version() >= EVersion::V4 && thread._threadName == nullptr) {
            ar << threadNameId;
        }
        FrameData data;
        ar << data._frameId;
        XXLOG_DETAIL("Load.frame(%d) for thread(%d)\n", data._frameId, threadId);
        _namePool.serialize(nullptr, ar);
        if (threadNameId != -1) {
            thread._threadName = name(*reinterpret_cast<const SName*>(&threadNameId));
        }
        ar << data._nodeCount;
        XXLOG_DETAIL("  nodeCount = %d\n", data._nodeCount);
        if (!ar.hasError() && data._nodeCount > 0) {
            size_t remainSize = sizeof(XXProfileTreeNode) * data._nodeCount;
            data._nodes = (XXProfileTreeNode*)malloc(remainSize);
            memset(data._nodes, 0, remainSize);
            if (ar.version() == EVersion::V1) {
                ar.serialize(data._nodes, remainSize);
            } else if (ar.version() >= EVersion::V2) {
                bool hasError = false;
                uint8_t* cur = (uint8_t*)data._nodes;
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
                    _dataSize += sizeOrg;
                    if (sizeCom) {
                        auto comBuf = buf.get(sizeCom < sizeOrg ? sizeOrg : sizeCom);
                        ar.serialize(comBuf, sizeCom);
                        if (ar.hasError()) {
                            break;
                        }
                        size_t destLen = decompress(cur, sizeOrg, comBuf, sizeCom);
                        if (destLen != sizeOrg) {
                            hasError = true;
                            break;
                        }
                        _fileSize += sizeCom;
                        XXLOG_DETAIL("%d: %lld => %lld %d => %d\n", data._frameId, totalFileSize, totalOrgSize, sizeCom, sizeOrg);
#if TEST_COMPRESS
                        {
                            uint32_t com = compressSize(compress, cur, sizeOrg, compressBuf);
                            uint32_t chunkedCom = compressSize(compressChunked, cur, sizeOrg, compressBuf);
                            fileSizeTest += com;
                            XXLOG_INFO("%d: chunked org: %d => %d com: %d vs %d\n", data._frameId, sizeOrg, sizeCom, com, chunkedCom);
                        }
#endif//TEST_COMPRESS
                    } else {
                        _fileSize += sizeOrg;
                        ar.serialize(cur, sizeOrg);
                        XXLOG_DETAIL("%d: %lld => %lld %d\n", data._frameId, totalFileSize, totalOrgSize, sizeOrg);
#if TEST_COMPRESS
                        {
                            uint32_t com = compressSize(compress, cur, sizeOrg, compressBuf);
                            uint32_t chunkedCom = compressSize(compressChunked, cur, sizeOrg, compressBuf);
                            fileSizeTest += com;
                            XXLOG_INFO("%d: chunked org: %d com: %d vs %d\n", data._frameId, sizeOrg, com, chunkedCom);
                        }
#endif//TEST_COMPRESS
                    }
                    cur += sizeOrg;
                    remainSize -= sizeOrg;
                    if (remainSize == 0) {
                        break;
                    }
                }
                if (hasError || remainSize != 0) {
                    break;
                }
            }
        }
        if (ar.hasError()) {
            break;
        }
        if (!data.init(*this)) {
            break;
        }
        if (thread._maxCycleCount < data.frameCycles()) {
            thread._maxCycleCount = data.frameCycles();
        }
        if (thread._maxCallDepth < data.maxCallDepth()) {
            thread._maxCallDepth = data.maxCallDepth();
        }
        thread._frames.push_back(std::move(data));
    }
#if TEST_COMPRESS
    XXLOG_INFO("Data compress: %d %02.2f%% %lld / %lld\n", ar.getCompressMethod(), (100.0 * _fileSize / _dataSize), _fileSize, _dataSize);
    XXLOG_INFO("Test compress: %02.2f%% %lld\n", (100.0 * fileSizeTest / _dataSize), fileSizeTest);
#endif//TEST_COMPRESS
    if (!_threads.empty()) {
        auto iter = _threads.begin();
        _processStart = iter->startTime();
        _processEnd = iter->endTime();
        for (const auto end = _threads.end(); iter != end; ++iter) {
            _processStart = std::min(_processStart, iter->startTime());
            _processEnd = std::max(_processStart, iter->startTime());
        }
    }

}

void Loader::clear() {
    _threads.clear();
    for (uint32_t i = 0, l = (uint32_t)_names.size(); i != l; ++i) {
        const char* name = _names[i];
        if (name && name != _namePool.getName(i + 1)) {
            free(const_cast<char*>(name));
        }
    }
    _names.clear();
    _namePool.clear();

    _secondsPerCycle = 0;
    _processStart = 0;
    _processEnd = 0;
    _dataSize = 0;
    _fileSize = 0;
}

const char* Loader::name(SName name) const {
    if (name.id() == 0) {
        return "";
    }
    uint32_t index = name.id() - 1;
    const char* n = nullptr;
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
    char* localName = nullptr;

    //if (strchr(name, "::__1::", )) {
    //    localName = strdup(name);
    //}
    //CppNameDecoder decoder(name);

    if (localName) {
        free(localName);
    }
    return name;
}

XX_NAMESPACE_END(xxprofile);
