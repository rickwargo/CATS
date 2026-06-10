#pragma once

#include <Arduino.h>
#include <Button2.h>

#include <utility>
#include "module/PinModule.h"
#include "Event.h"

class ButtonModule : public PinModule
{
public:
    ButtonModule(std::string name, short pin)
    : PinModule(std::move(name), pin) {}

protected:
    bool setup() override
    {
        pinMode(pin, INPUT_PULLUP);     // If attached to a read-only pin, this should only be INPUT
        button.begin(pin);
        button.setLongClickTime(333);

        button.setClickHandler([this](Button2&) {
            onClick();
        });
        button.setLongClickHandler([this](Button2&) {
            onLongClick();
        });
        button.setDoubleClickHandler([this](Button2&) {
            onDoubleClick();
        });
        button.setTripleClickHandler([this](Button2&) {
            onTripleClick();
        });

        return true;
    }

    void cycle() override
    {
        button.loop();
    }

    virtual void onClick() {
        if (ctx.bus) {
            ctx.bus->publish({Event::ButtonClick, this});
        }
    }

    virtual void onLongClick() {
        if (ctx.bus) {
            ctx.bus->publish({Event::SystemReset, this});
            // ctx.bus->publish({Event::ButtonLong, this});
        }
    }

    virtual void onDoubleClick() {
        if (ctx.bus) {
            ctx.bus->publish({Event::ButtonDouble, this});
        }
    }

    virtual void onTripleClick() {
        if (ctx.bus) {
            ctx.bus->publish({Event::ButtonTriple, this});
        }
    }

private:
    Button2 button;
};
