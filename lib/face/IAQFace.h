#pragma once
// Difference between IAQ and AQI: https://shop.smartviewaqi.com/understanding-difference-between-aqi-vs-iaq/

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"

class IAQFace : public FaceModule, ISubscriber
{
public:
    IAQFace(const char* name = "IAQ") : FaceModule(name) {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::IAQChanged)
            measure.set(e.data);
    }

protected:
    Measure<float> measure{0};

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
