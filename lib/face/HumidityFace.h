#pragma once

#include "NumericMeasure.h"
#include "ISubscriber.h"
#include "Event.h"


class HumidityFace : public NumericMeasure, ISubscriber
{
public:
    HumidityFace(const char* name, unsigned long cycleCheckTime)
    : NumericMeasure("Humidity", cycleCheckTime,"%") {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::HumidityChanged) setMeasureValue(e.data / 10.0);
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }
};
