#pragma once

#include <Arduino.h>
#include "Module.h"
#include "Event.h"
#include "say.h"

class SystemModule : public Module, public ISubscriber
{
public:
    SystemModule(std::string name) : Module(std::move(name)) {}

    void onEvent(Event& e) override
    {
        static time_t lastEncoderEvent = 0;

#if CORE_DEBUG_LEVEL >= 0
        say("[Event System] type: %s, data: %d", e.getName(), e.data);
#endif
        // Event Dispatch System
        switch (e.type) {
        case Event::ButtonTriple:
            e.type = Event::SystemReset;    // not really necessary but documents a triple click induces a system reset
        case Event::SystemReset:
            say("System reset requested (reason: #%d)", resetReasons[e.data]);
#if CORE_DEBUG_LEVEL >= 3
            // Give a bit of time to see this event before rebooting
            delay(2000);
#endif
            ESP.restart();
            break;
        case Event::ButtonLong:
            // If pressing and turning, ignore the long click event
            e.type = Event::ToggleMusic;
            // e.type = e.timestamp - lastEncoderEvent > 1000 ? Event::ToggleMusic : Event::EventDiscarded;
            break;
        case Event::ButtonDouble:
            e.type = Event::NextMusic;
            break;
        case Event::EncoderCCW:
        case Event::EncoderCW:
            // lastEncoderEvent = e.timestamp;
            break;
        default:
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
    const char *resetReasons[1] = {
        "On demand reset"
    };
};

// ctx.bus->publish({Event::ToggleMusic, this});
