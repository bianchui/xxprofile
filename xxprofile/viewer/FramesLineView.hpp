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
        FramesItemHeight = 100,
        PixelsPerFrame = 2,
        PixelsPerThumbnail = 2,
    };

    struct ThreadData {
        const xxprofile::ThreadData* _data;

        // frames
        int _startIndex;
        int _selectedItem;
        int _selectedCount;
        int _maxItemCount;

        // thumbnail
        int _framesPerThumbnail;
        std::vector<double> _timesForThumbnail;
        int _thumbnailSelectedCount;

        void init(const xxprofile::ThreadData* data) {
            assert(data);
            _data = data;

            // frames
            _startIndex = 0;
            _selectedItem = 0;
            _selectedCount = 0;
            _maxItemCount = 0;

            // thumbnail
            _framesPerThumbnail = 1;
            _timesForThumbnail.clear();
            _thumbnailSelectedCount = 0;
        }
        void setGraphWidthPixels(int pixels) {
            if (pixels < PixelsPerFrame) {
                return;
            }
            const auto& frames_ = _data->_frames;
            const int maxItemCount = pixels / PixelsPerFrame;
            const int frameCount = (int)frames_.size();
            const int framesPerThumbnail = (frameCount + maxItemCount - 1) / maxItemCount;
            if (_framesPerThumbnail != framesPerThumbnail) {
                _framesPerThumbnail = framesPerThumbnail;
                // thumbnail enabled
                if (hasThumbnail()) {
                    const int timeCount = (int)((frameCount + framesPerThumbnail - 1) / framesPerThumbnail);
                    _timesForThumbnail.resize(timeCount);
                    const auto* frames = frames_.data();
                    double* timesForThumbnail = _timesForThumbnail.data();
                    int remainFrames = frameCount;
                    for (int i = 0; i < timeCount; ++i) {
                        uint64_t maxCycles = frames[0].frameCycles();
                        const int thumbnailFrameCount = remainFrames > framesPerThumbnail ? framesPerThumbnail : remainFrames;
                        for (int f = 1; f < thumbnailFrameCount; ++f) {
                            const uint64_t cycles = frames[f].frameCycles();
                            if (maxCycles < cycles) {
                                maxCycles = cycles;
                            }
                        }
                        timesForThumbnail[i] = maxCycles * _data->_secondsPerCycle;
                        frames += framesPerThumbnail;
                        remainFrames -= framesPerThumbnail;
                    }
                }
            }
            _thumbnailSelectedCount = ((int)_timesForThumbnail.size()) * maxItemCount / frameCount;
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
            const auto& frames = _data->_frames;
            const int index = _startIndex + idx;
            if (index < frames.size()) {
                return frames[index].frameCycles() * _data->_secondsPerCycle;
            } else {
                assert(false);
                return 0;
            }
        }
        static float StaticFramesGetData(void* p, int idx) {
            return (float)((const ThreadData*)p)->data(idx);
        }
        void framesFormat(shared::StrBuf& buf, int idx) const {
            const auto& frames = _data->_frames;
            int index = idx + _startIndex;
            if (index < frames.size()) {
                const auto& frame = frames[index];
                double time = frame.frameCycles() * _data->_secondsPerCycle;
                buf.appendf("%d: ", frame.frameId());
                Math::FormatTime(buf, time);
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
            plot.valuesCount = (int)_data->_frames.size();
            if (plot.valuesCount > _maxItemCount) {
                plot.valuesCount = _maxItemCount;
            }
            plot.selectedItem = _selectedItem - _startIndex;
            plot.selectedCount = 1;
        }
        void setFramesTrackingItem(int item) {
            _selectedItem = item + _startIndex;
        }
        void getFramesOverlay(shared::StrBuf& buf) const {
            int endIndex = _startIndex + _maxItemCount;
            if (endIndex >= (int)_data->_frames.size()) {
                endIndex = (int)_data->_frames.size() - 1;
            }
            assert(_startIndex < (int)_data->_frames.size());
            const auto& start = _data->_frames[_startIndex];
            const auto& end = _data->_frames[endIndex];
            buf.printf("[%d, %d]", start.frameId(), end.frameId());
            Math::FormatTime(buf, frameUseTime());
        }

        // thumbnail
        bool hasThumbnail() const {
            return _framesPerThumbnail > 1;
        }
        double thumbnailGetData(int idx) const {
            assert(idx < _timesForThumbnail.size());
            return _timesForThumbnail[idx];
        }
        static float StaticThumbnailGetData(void* p, int idx) {
            return (float)((const ThreadData*)p)->thumbnailGetData(idx);
        }
        void thumbnailFormat(shared::StrBuf& buf, int idx) const {
            assert(idx < _timesForThumbnail.size());
            double v = _timesForThumbnail[idx];
            const int index = idx * _framesPerThumbnail;
            int endIndex = index + _framesPerThumbnail - 1;
            if (endIndex >= (int)_data->_frames.size()) {
                endIndex = (int)_data->_frames.size() - 1;
            }
            assert(index < (int)_data->_frames.size());
            const auto& start = _data->_frames[index];
            const auto& end = _data->_frames[endIndex];
            buf.printf("[%d, %d]\n", start.frameId(), end.frameId());
            Math::FormatTime(buf, v);
        }
        static void StaticThumbnailFormat(void* p, shared::StrBuf& buf, int idx) {
            ((const ThreadData*)p)->thumbnailFormat(buf, idx);
        }
        void setThumbnail(ImGui::ImPlotWithHitTest& plot) const {
            plot.data = (void*)this;
            plot.formatValue = StaticThumbnailFormat;
            plot.valuesGetter = StaticThumbnailGetData;
            plot.scaleMax = (float)(_data->_maxCycleCount * _data->_secondsPerCycle);
            plot.valuesCount = (int)_timesForThumbnail.size();
            plot.selectedItem = _startIndex / _framesPerThumbnail;
            plot.selectedCount = _thumbnailSelectedCount;
        }
        void setThumbnailTrackingItem(int item) {
            int minI = item - _thumbnailSelectedCount / 2;
            int maxI = minI + _thumbnailSelectedCount;

            if (minI < 0) {
                maxI = _thumbnailSelectedCount;
                minI = 0;
            } else if (maxI > _timesForThumbnail.size()) {
                maxI = (int)_timesForThumbnail.size();
                minI = maxI - _thumbnailSelectedCount;
            }
            int startIndex = minI * _framesPerThumbnail;
            int endIndex = startIndex + _maxItemCount;
            const int frameCount = (int)_data->_frames.size();
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
            printf("    _thumbnailCount = %d\n", (int)_timesForThumbnail.size());
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
