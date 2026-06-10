#pragma once

#include "Event.h"

class ISubscriber {
public:
    virtual ~ISubscriber() = default;
    virtual void onEvent(const Event& e) = 0;
    virtual bool ignoreEvents() { return false; }
};
