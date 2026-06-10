#pragma once

#include <Arduino.h>
#include <SparkFun_VEML7700_Arduino_Library.h>

#include <utility>
#include "module/I2CModule.h"
#include "mathHelpers.h"

class LightLevelModule : public I2CModule
{
public:
    LightLevelModule(std::string name, unsigned int cycleCheckTime) : I2CModule(std::move(name), cycleCheckTime) {}

    static constexpr short LEVEL_DARK = 50;
    static constexpr short LEVEL_LIGHT = 300;
    static constexpr float MINIMUM_PERCENT_CHANGE = 0.10;

protected:
    bool setup() override
    {
        if (lightLevelSensor.begin() == false)
        {
            say("Unable to communicate with the VEML7700. Please check the wiring.");
            return false;
        }

        return true;
    }

    void cycle() override
    {
        short lux = lightLevelSensor.getLux();

        // Light/Dark events occur prior to light level change events
        if (lux < LEVEL_DARK && previousLux >= LEVEL_DARK) onDark(lux);
        if (lux >= LEVEL_LIGHT && previousLux < LEVEL_LIGHT) onLight(lux);

        // NOTE: small, gradual light changes could enter dark or light mode without publishing an event
        if (lux != previousLux && abs(percentChange(previousLux, lux)) >= MINIMUM_PERCENT_CHANGE)
            onChange(lux);

        previousLux = lux;
    }

    virtual void onChange(short lux) {
        if (ctx.bus) {
            ctx.bus->publish({Event::LightLevelChanged, this, lux});
        }
    }

    virtual void onDark(short lux) {
        if (ctx.bus) {
            ctx.bus->publish({Event::LightLevelDark, this, lux});
        }
    }

    virtual void onLight(short lux) {
        if (ctx.bus) {
            ctx.bus->publish({Event::LightLevelLight, this, lux});
        }
    }

private:
    short previousLux = -1111;
    SparkFunVEML7700 lightLevelSensor; // Create a VEML7700 object
};
