#pragma once

#include "NumericMeasure.h"
#include "ISubscriber.h"
#include "Event.h"


// Difference between IAQ and AQI: https://shop.smartviewaqi.com/understanding-difference-between-aqi-vs-iaq/

class IAQFace : public NumericMeasure, ISubscriber
{
public:
    IAQFace(const char* name, unsigned long cycleCheckTime)
        : NumericMeasure("IAQ", cycleCheckTime) {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::IAQChanged)
        {
            if (e.data > 0)
                setMeasureValue(e.data);
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }
};
