#pragma once

#include "ISubscriber.h"
#include "Event.h"

class EventBus {
public:
    static constexpr int MAX_SUBS = 32;

    void subscribe(ISubscriber* sub) {
        if (count < MAX_SUBS) {
            subs[count++] = sub;
        }
    }

    void publish(const Event& e) {
        Event event(e.type, e.source, e.data);
        for (int i = 0; i < count; i++) {
            if (!subs[i]->ignoreEvents() && e.type != Event::EventDiscarded)
                subs[i]->onEvent(event);
        }
    }

private:
    ISubscriber* subs[MAX_SUBS] = {nullptr};
    int count = 0;
};
