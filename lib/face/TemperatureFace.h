#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"


class TemperatureFace : public FaceModule, ISubscriber
{
public:
    TemperatureFace(const char* name = "Temperature") : FaceModule(name) {}

    void onEvent(Event& e) override
    {
        if (e.type == Event::TemperatureChanged)
            measure.set(e.data / 10.0);
    }

protected:
    Measure<float> measure{0.0f, "`F", 1};

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
