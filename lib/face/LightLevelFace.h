#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"


class LightLevelFace : public FaceModule, ISubscriber
{
public:
    LightLevelFace(const char* name = "Light Level") : FaceModule(name) {}

    void onEvent(Event& e) override
    {
        if (e.type == Event::LightLevelChanged)
            measure.set(e.data);
    }

protected:
    Measure<short> measure{-111, "lux"};

    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }

    void renderFace() override
    {
        renderMeasureToSprite(&measure);
    }
};
