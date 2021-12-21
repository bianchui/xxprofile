// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofileviewer_FramesLineView_hpp
#define xxprofileviewer_FramesLineView_hpp
#include "../loader/xxprofile_loader.hpp"
#include "imgui/imgui_custom.hpp"
#include "EventHandler.hpp"
#include <cmath>

class FramesLineView {
public:
    enum {
        ThumbnailItemHeight = 20,
        FramesItemHeight = 80,
        PixelsPerFrame = 2,
        PixelsPerThumbnail = 2,
    };

    struct ThreadData {
        const xxprofile::ThreadData* _data;
        std::vector<const xxprofile::FrameData*> _frames;

        uint32_t _minFrameId;
        uint32_t _maxFrameId;

        bool _focus;

        // frames
        int _startIndex;
        int _selectedItem;
        int _selectedCount;
        int _maxItemCount;

        // thumbnail
        int _framesPerThumbnail;
        struct Thumbnail {
            double data;
            uint32_t minFrameId;
            uint32_t maxFrameId;
        };
        std::vector<Thumbnail> _thumbnail;
        int _thumbnailSelectedCount;

        void init(const xxprofile::ThreadData* data, uint32_t minFrameId, uint32_t maxFrameId) {
            assert(data);
            _focus = false;
            _data = data;
            _minFrameId = minFrameId;
            _maxFrameId = maxFrameId;
            _frames.resize(maxFrameId - minFrameId + 1);

            for (const xxprofile::FrameData& frame : data->_frames) {
                assert(minFrameId <= frame.frameId());
                assert(frame.frameId() <= maxFrameId);
                _frames[frame.frameId() - minFrameId] = &frame;
            }

            // frames
            _startIndex = 0;
            _selectedItem = 0;
            _selectedCount = 0;
            _maxItemCount = 0;

            // thumbnail
            _framesPerThumbnail = 1;
            _thumbnail.clear();
            _thumbnailSelectedCount = 0;
        }
        void setGraphWidthPixels(int pixels) {
            if (pixels < PixelsPerFrame) {
                return;
            }
            const int maxItemCount = pixels / PixelsPerFrame;
            const int frameCount = (int)_frames.size();
            const int framesPerThumbnail = (frameCount + maxItemCount - 1) / maxItemCount;
            if (_framesPerThumbnail != framesPerThumbnail) {
                _framesPerThumbnail = framesPerThumbnail;
                // thumbnail enabled
                if (hasThumbnail()) {
                    const int timeCount = (int)((frameCount + framesPerThumbnail - 1) / framesPerThumbnail);
                    _thumbnail.resize(timeCount);
                    const auto* frames = _data->_frames.data();
                    const auto* end_frames = frames + _data->_frames.size();
                    auto* thumbnail = _thumbnail.data();
                    for (int i = 0; i < timeCount; ++i) {
                        const uint32_t minFrameId = _minFrameId + framesPerThumbnail * i;
                        uint32_t endFrame = minFrameId + framesPerThumbnail;
                        uint64_t maxCycles = 0;
                        uint32_t maxFrameId = minFrameId;
                        uint32_t frameCount = 0;
                        while (frames < end_frames && frames->frameId() < endFrame) {
                            const uint64_t cycles = frames->frameCycles();
                            if (maxCycles < cycles) {
                                maxCycles = cycles;
                            }
                            maxFrameId = frames->frameId();
                            ++frames;
                            ++frameCount;
                        }
                        thumbnail[i].data = maxCycles * _data->_secondsPerCycle;
                        if (frameCount == 0) {
                            thumbnail[i].minFrameId = -1;
                            thumbnail[i].maxFrameId = -1;
                        } else {
                            thumbnail[i].minFrameId = minFrameId;
                            thumbnail[i].maxFrameId = maxFrameId;
                        }
                    }
                }
            }
            _thumbnailSelectedCount = ((int)_thumbnail.size()) * maxItemCount / frameCount;
            if (_thumbnailSelectedCount <= 1) {
                _thumbnailSelectedCount = 1;
            }
            if (_maxItemCount != maxItemCount) {
                // update frames start index
                if (hasThumbnail()) {
                    // try not change startIndex;
                    int startIndex = _startIndex;
                    assert(_selectedItem < frameCount);
                    if (startIndex + maxItemCount > frameCount) {
                        startIndex = frameCount - maxItemCount;
                    }
#if 0
                    if (_selectedItem < startIndex) {
                        startIndex = _selectedItem;
                    }
                    if (startIndex + maxItemCount < _selectedItem) {
                        startIndex = _selectedItem - maxItemCount;
                    }
#endif//0
                    _startIndex = startIndex;
                } else {
                    _startIndex = 0;
                }
                _maxItemCount = maxItemCount;

                //dump();
                //printf("    width = %d\n", pixels);
            }
        }
        double frameUseTime() const {
            return _data->_maxCycleCount * _data->_secondsPerCycle;
        }

        // frames
        double data(int idx) const {
            const auto& frames = _frames;
            const int index = _startIndex + idx;
            if (index < frames.size()) {
                const auto* frame = frames[index];
                return frame ? frame->frameCycles() * _data->_secondsPerCycle : 0;
            } else {
                assert(false);
                return 0;
            }
        }
        static float StaticFramesGetData(void* p, int idx) {
            return (float)((const ThreadData*)p)->data(idx);
        }
        void framesFormat(shared::StrBuf& buf, int idx) const {
            const auto& frames = _frames;
            int index = idx + _startIndex;
            if (index < frames.size()) {
                const auto* frame = frames[index];
                if (frame) {
                    double time = frame->frameCycles() * _data->_secondsPerCycle;
                    buf.appendf("%d: ", frame->frameId());
                    Math::FormatTime(buf, time);
                } else {
                    buf.appendf("no frame");
                }
            } else {
                assert(false);
                buf.append("Error index %d", index);
            }
        }
        static void StaticFramesFormat(void* p, shared::StrBuf& buf, int idx) {
            return ((const ThreadData*)p)->framesFormat(buf, idx);
        }
        void setFrames(ImGui::ImPlotWithHitTest& plot) const {
            plot.data = (void*)this;
            plot.formatValue = StaticFramesFormat;
            plot.valuesGetter = StaticFramesGetData;
            plot.scaleMax = (float)(_data->_maxCycleCount * _data->_secondsPerCycle);
            plot.valuesCount = (int)_frames.size();
            if (plot.valuesCount > _maxItemCount) {
                plot.valuesCount = _maxItemCount;
            }
            if (_focus) {
                plot.selectedItem = _selectedItem - _startIndex;
                plot.selectedCount = 1;
            } else {
                plot.selectedItem = -1;
                plot.selectedCount = 0;
            }
        }
        void setFramesTrackingItem(int item) {
            _selectedItem = item + _startIndex;
            setFocus(true);
        }
        void setFocus(bool b) {
            _focus = b;
        }
        const xxprofile::FrameData* getTrackingFrame() const {
            return _frames[_selectedItem];
        }
        void getFramesOverlay(shared::StrBuf& buf) const {
            assert(_startIndex < (int)_frames.size());
            int endIndex = _startIndex + _maxItemCount;
            if (endIndex >= (int)_frames.size()) {
                endIndex = (int)_frames.size() - 1;
            }
            buf.printf("Thread %d [%d, %d]", _data->_threadId, _minFrameId + _startIndex, _minFrameId + endIndex);
            Math::FormatTime(buf, frameUseTime());
        }

        // thumbnail
        bool hasThumbnail() const {
            return _framesPerThumbnail > 1;
        }
        double thumbnailGetData(int idx) const {
            assert(idx < _thumbnail.size());
            return _thumbnail[idx].data;
        }
        static float StaticThumbnailGetData(void* p, int idx) {
            return (float)((const ThreadData*)p)->thumbnailGetData(idx);
        }
        void thumbnailFormat(shared::StrBuf& buf, int idx) const {
            assert(idx < _thumbnail.size());
            const auto& v = _thumbnail[idx];
            if (v.minFrameId != -1) {
                buf.printf("[%d, %d]\n", v.minFrameId, v.maxFrameId);
                Math::FormatTime(buf, v.data);
            } else {
                buf.printf("no frames");
            }
        }
        static void StaticThumbnailFormat(void* p, shared::StrBuf& buf, int idx) {
            ((const ThreadData*)p)->thumbnailFormat(buf, idx);
        }
        void setThumbnail(ImGui::ImPlotWithHitTest& plot) const {
            plot.data = (void*)this;
            plot.formatValue = StaticThumbnailFormat;
            plot.valuesGetter = StaticThumbnailGetData;
            plot.scaleMax = (float)(_data->_maxCycleCount * _data->_secondsPerCycle);
            plot.valuesCount = (int)_thumbnail.size();
            plot.selectedItem = _startIndex / _framesPerThumbnail;
            plot.selectedCount = _thumbnailSelectedCount;
        }
        void setThumbnailTrackingItem(int item) {
            int minI = item - _thumbnailSelectedCount / 2;
            int maxI = minI + _thumbnailSelectedCount;

            if (minI < 0) {
                maxI = _thumbnailSelectedCount;
                minI = 0;
            } else if (maxI > _thumbnail.size()) {
                maxI = (int)_thumbnail.size();
                minI = maxI - _thumbnailSelectedCount;
            }
            int startIndex = minI * _framesPerThumbnail;
            int endIndex = startIndex + _maxItemCount;
            const int frameCount = (int)_frames.size();
            if (endIndex > frameCount) {
                startIndex = frameCount - _maxItemCount;
                endIndex = frameCount;
            }
            _startIndex = startIndex;
            //if (_selectedItem < startIndex) {
            //    _selectedItem = startIndex;
            //} else if (endIndex < _selectedItem) {
            //    _selectedItem = endIndex;
            //}
        }

        void dump() const {
            printf("ThreadData\n");
            printf("    _frameCount = %d\n", (int)_data->_frames.size());
            // frames
            printf("    _startIndex = %d\n", _startIndex);
            printf("    _selectedItem = %d\n", _selectedItem);
            printf("    _selectedCount = %d\n", _selectedCount);
            printf("    _maxItemCount = %d\n", _maxItemCount);
            // thumbnail
            printf("    _framesPerThumbnail = %d\n", _framesPerThumbnail);
            printf("    _thumbnailCount = %d\n", (int)_thumbnail.size());
            printf("    _thumbnailSelectedCount = %d\n", _thumbnailSelectedCount);
        }
    };

    FramesLineView(EventHandler* handler);
    ~FramesLineView();

    float calcHeight(int framesGraphWidthPixels);
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
};

#endif//xxprofileviewer_FramesLineView_hpp
