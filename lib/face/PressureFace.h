#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"
#include "UnitConverter.h"


class PressureFace : public FaceModule, ISubscriber
{
public:
    PressureFace(const char* name = "Pressure") : FaceModule(name) {}

    void onEvent(Event& e) override
    {
        if (e.type == Event::PressureChanged)
            measure.set((e.data / 10.0 + mbarToInHg.offset) * mbarToInHg.scale);
    }

protected:
    Measure<float> measure{0.0f, "inHg", 2};

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
