//
//  TimeLineView.cpp
//  xxprofileViewer
//
//  Created by bianchui on 2025/1/7.
//  Copyright © 2025 bianchui. All rights reserved.
//

#include "TimeLineView.hpp"

TimeLineView::TimeLineView(EventHandler* handler) : _handler(handler), _loader(nullptr) {
}

TimeLineView::~TimeLineView() {

}

float TimeLineView::getHeight() {
    return 0;
}

void TimeLineView::setLoader(const xxprofile::Loader* loader) {
    clear();
    _loader = loader;
    if (loader) {
    }
}

void TimeLineView::clear() {

}

void TimeLineView::draw() {
    if (_threads.empty()) {
        return;
    }
}
