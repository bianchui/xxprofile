//
//  TimeLineView.hpp
//  xxprofileViewer
//
//  Created by bianchui on 2025/1/7.
//  Copyright © 2025 bianchui. All rights reserved.
//

#ifndef TimeLineView_hpp
#define TimeLineView_hpp
#include "../../loader/xxprofile_loader.hpp"
#include "../EventHandler.hpp"

class TimeLineView {
public:
    struct ThreadData {
        const xxprofile::ThreadData* _data;

        bool _expended;
    };

    TimeLineView(EventHandler* handler);
    ~TimeLineView();

    float getHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
    float _timelineScale;
    float _timelineOffset;
    float _scrollBarSize = 15.0f;
    float _barHeight = 15.f;
    float _maxTime = 80;
};

#endif /* TimeLineView_hpp */
