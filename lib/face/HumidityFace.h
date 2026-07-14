#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"


class HumidityFace : public FaceModule, ISubscriber
{
public:
    HumidityFace(const char* name = "Humidity") : FaceModule(name) {}

    void onEvent(Event& e) override
    {
        if (e.type == Event::HumidityChanged)
            measure.set(e.data / 10.0);
    }

protected:
    Measure<float> measure{0.0f, "%", 1};

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
