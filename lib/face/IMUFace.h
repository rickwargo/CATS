#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "Measure.h"


class IMUFace : public FaceModule, ISubscriber
{
public:
    IMUFace(const char* name = "Y/P/R") : FaceModule(name) {}

    void onEvent(const Event& e) override
    {
        if (e.type == Event::YawChanged)
            yaw.set(e.data);
        else if (e.type == Event::PitchChanged)
            pitch.set(e.data);
        else if (e.type == Event::RollChanged)
            roll.set(e.data);
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }

    void renderFace() override
    {
        renderMeasuresToSprite(ypr, "Y / P / R");
    }

private:
    Measure<short> yaw{-1, "deg"};
    Measure<short> pitch{-1, "deg"};
    Measure<short> roll{-1, "deg"};
    std::vector<IMeasure*> ypr{&yaw, &pitch, &roll};
};
