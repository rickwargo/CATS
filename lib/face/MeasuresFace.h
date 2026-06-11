#pragma once
#include "FaceModule.h"
#include "Measure.h"
#include "ISubscriber.h"
#include "Event.h"
#include "UnitConverter.h"


class MeasuresFace : public FaceModule, ISubscriber
{
public:
    MeasuresFace(const char* name) : FaceModule(name) {}

    void onEvent(const Event& e) override
    {
        switch (e.type)
        {
        case Event::TemperatureChanged:
            temperature.set(e.data / 10.0);
            currentMeasure = &temperature;
            break;
        case Event::HumidityChanged:
            humidity.set(e.data / 10.0);
            currentMeasure = &humidity;
            break;
        case Event::LightLevelChanged:
            humidity.set(e.data);
            currentMeasure = &lightLevel;
            break;
        case Event::PressureChanged:
            pressure.set((e.data / 10.0 + mbarToInHg.offset) * mbarToInHg.scale);
            currentMeasure = &pressure;
            break;
        case Event::IAQChanged:
            if (e.data > 0)
            {
                iaq.set(e.data);
                currentMeasure = &iaq;
            };
            break;
        case Event::YawChanged:
            yaw.set(e.data);
            currentMeasure = &yaw;
            break;
        case Event::PitchChanged:
            pitch.set(e.data);
            currentMeasure = &pitch;
            break;
        case Event::RollChanged:
            roll.set(e.data);
            currentMeasure = &roll;
            break;
        default:
            currentMeasure = nullptr;
            break;
        }
        measureName = e.getName();
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        return true;
    }

    void renderFace() override
    {
        if (currentMeasure != nullptr) renderMeasureToSprite(currentMeasure, measureName);
    }

    void cycle() override
    {
        if (isActiveFace() && currentMeasure != nullptr) drawFace();
    }

private:
    Measure<float> temperature{0.0f, "F", 1};
    Measure<float> humidity{0.0f, "%", 1};
    Measure<float> pressure{0.0f, "inHg", 2};
    Measure<short> lightLevel{-111, "lux"};
    Measure<short> iaq{-1};
    Measure<short> yaw{-1, "deg"};
    Measure<short> pitch{-1, "deg"};
    Measure<short> roll{-1, "deg"};
    IMeasure* currentMeasure{nullptr};
    const char *measureName = nullptr;
};
