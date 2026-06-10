#pragma once

#include "NumericMeasure.h"
#include "ISubscriber.h"
#include "Event.h"


class PressureFace : public NumericMeasure, ISubscriber
{
public:
    PressureFace(const char* name, unsigned long cycleCheckTime) : NumericMeasure("Pressure", cycleCheckTime, "inHg") {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::PressureChanged) setMeasureValue(toInHg(e.data / 10.0f));
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }

private:
    static float toInHg(float val) { return val * 0.02952998057228f; };

};
