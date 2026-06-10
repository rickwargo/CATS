#pragma once

#include "NumericMeasure.h"


class TemperatureFace : public NumericMeasure, ISubscriber
{
public:
    TemperatureFace(const char*, unsigned long cycleCheckTime)
    : NumericMeasure("Temperature", cycleCheckTime, "F") {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::TemperatureChanged) setMeasureValue(e.data / 10.0);
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }
};
