#pragma once

#include <Arduino.h>

#include <utility>
#include "Event.h"
#include "PinModule.h"
#include "say.h"

class HapticModule : public PinModule, public ISubscriber
{
public:
    HapticModule(std::string name, short pin) : PinModule(std::move(name), pin) {}

    void onEvent(Event& e) override
    {
        switch (e.type) {
        case Event::HapticFeedbackRequest:
            onFeedbackRequest((short)e.data);
            break;
        default:
            break;
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);

        return true;
    }

    virtual void onFeedbackRequest(short data) {
        vibrate(pin, data);
    }

private:
    void vibrate(short pin, short direction = 0, short strength = 100, bool fade = false, short duration_ms = 100)
    {
        if (strength > 100) strength = 100;
        if (strength < 0) strength = 0;
        if (direction >= 360) direction = 0;
        if (direction < 0) direction += 360;

        say("[%s] vibrate pin %d", getName(), pin);
        // TODO: Need to replace delay() mechanism
        digitalWrite(pin, HIGH * strength / 100);
        delay(duration_ms);
        digitalWrite(pin, LOW);
    }
};
