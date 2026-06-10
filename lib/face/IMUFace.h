#pragma once

#include "NumericMeasure.h"
#include "ISubscriber.h"
#include "Event.h"


class IMUFace : public NumericMeasure, ISubscriber
{
public:
    IMUFace(const char* name, unsigned long cycleCheckTime) : NumericMeasure("Y/P/R", cycleCheckTime, "deg") {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::YawChanged) measures[0] = e.data / 10.0;
        else if (e.type == Event::PitchChanged) measures[1] = e.data / 10.0;
        else if (e.type == Event::RollChanged) measures[2] = e.data / 10.0;
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);
        backgroundProcessing = true;    // keep yaw / pitch / roll up-to-date
        setMeasures({0.0f, 0.0f, 0.0f});

        return true;
    }
};
