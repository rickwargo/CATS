#pragma once

#include <Arduino.h>
#include "Module.h"
#include "Event.h"
#include "say.h"

class SystemModule : public Module, public ISubscriber
{
public:
    SystemModule(std::string name) : Module(std::move(name)) {}

    void onEvent(const Event& e) override
    {
        // Dispatch System
        switch (e.type) {
        // Lock in system-type functionality
        case Event::SystemReset:
            ESP.restart();
            break;
        // And dispatch the other events
        default:
            Event dispatch = dispatchEvent(e);
            // ctx.bus->publish(dispatch);
            break;
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }

private:

    // This should dispatch to the active screen, and elsewhere as needed
    Event dispatchEvent(const Event &event)
    {
        switch (event.type)
        {
        case Event::SystemReset:
            say("System reset requested: reason: #%d", event.data);
            ESP.restart();
            break;
        default:
            // say("[dispatchEvent] event: %s, data: %d", event.getName(), event.data);
            break;
        }
        return event;
    }

};
