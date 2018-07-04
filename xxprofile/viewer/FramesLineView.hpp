// Copyright 2017 bianchui. All rights reserved.
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
        int _thumbnailSelectedItem;
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
            _thumbnailSelectedItem = 0;
            _thumbnailSelectedCount = 0;
        }
        void setGraphWidthPixels(int pixels) {
            const auto& frames_ = _data->_frames;
            _maxItemCount = pixels / PixelsPerFrame;
            int frameCount = (int)frames_.size();
            const int framesPerThumbnail = (frameCount + pixels - 1) / pixels;
            if (_framesPerThumbnail != framesPerThumbnail) {
                _framesPerThumbnail = framesPerThumbnail;
                // thumbnail enabled
                if (framesPerThumbnail != 1) {
                    const int timeCount = (int)((frameCount + framesPerThumbnail - 1) / framesPerThumbnail);
                    _timesForThumbnail.resize(timeCount);
                    const auto* frames = frames_.data();
                    double* timesForThumbnail = _timesForThumbnail.data();
                    for (int i = 0; i < timeCount; ++i) {
                        uint64_t maxCycles = frames[0].frameCycles();
                        const int thumbnailFrameCount = frameCount > framesPerThumbnail ? framesPerThumbnail : frameCount;
                        for (int f = 1; f < thumbnailFrameCount; ++f) {
                            const uint64_t cycles = frames[f].frameCycles();
                            if (maxCycles < cycles) {
                                maxCycles = cycles;
                            }
                        }
                        timesForThumbnail[i] = maxCycles * _data->_secondsPerCycle;
                        frames += framesPerThumbnail;
                        frameCount -= framesPerThumbnail;
                    }

                    //_thumbnailSelectedCount =
                }
                // update frames start index
                {

                }
            }
        }
        double frameUseTime() const {
            return _data->_maxCycleCount * _data->_secondsPerCycle;
        }

        // frames
        double data(int idx) const {
            const auto& frames = _data->_frames;
            const int index = _startIndex + idx;
            const xxprofile::FrameData& frameData = frames[index % frames.size()];
            return frameData.frameCycles() * _data->_secondsPerCycle;
        }
        static float StaticFramesGetData(void* p, int idx) {
            const ThreadData* data = (const ThreadData*)p;
            return (float)data->data(idx);
        }
        static void StaticFramesFormat(void* p, shared::StrBuf& buf, int idx) {
            const ThreadData* data = (const ThreadData*)p;
            double v = data->data(idx);
            buf.appendf("%d: ", idx + data->_startIndex + 1);
            Math::FormatTime(buf, v);
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
        void setStartIndex(int startIndex) {
            const auto& frames = _data->_frames;

        }

        // thumbnail
        bool hasThumbnail() const {
            return _framesPerThumbnail > 1;
        }
        static float StaticThumbnailGetData(void* p, int idx) {
            const ThreadData* data = (const ThreadData*)p;
            return (float)data->_timesForThumbnail[idx];
        }
        static void StaticThumbnailFormat(void* p, shared::StrBuf& buf, int idx) {
            const ThreadData* data = (const ThreadData*)p;
            double v = data->_timesForThumbnail[idx];

            buf.appendf("%d: ", idx + 1);
            Math::FormatTime(buf, v);
        }
        void setThumbnail(ImGui::ImPlotWithHitTest& plot) const {
            plot.data = (void*)this;
            plot.formatValue = StaticThumbnailFormat;
            plot.valuesGetter = StaticThumbnailGetData;
            plot.scaleMax = (float)(_data->_maxCycleCount * _data->_secondsPerCycle);
            plot.valuesCount = (int)_timesForThumbnail.size();
            plot.selectedItem = _thumbnailSelectedItem;
            plot.selectedCount = _thumbnailSelectedCount;
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
