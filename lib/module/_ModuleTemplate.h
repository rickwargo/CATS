#pragma once

#include "Event.h"
#include "Module.h"
// #include "say.h"

class TemplateModule : public Module, public ISubscriber
{
public:
    TemplateModule(std::string name) : Module(std::move(name)) {}

    void onEvent(Event& e) override
    {
        switch (e.type) {
        case Event::LastType:   // Essentially a NOOP
            onUpdateRequest((short)e.data);
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

    void cycle() override
    {
    }

    virtual void onUpdateRequest(short data) {
    }

private:
};
