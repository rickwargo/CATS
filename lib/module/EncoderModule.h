#pragma once

#include "AiEsp32RotaryEncoder.h"
#include "Module.h"
#include "Event.h"

class EncoderModule : public Module
{
public:
    const int STEP_COUNT = 192;

    EncoderModule(std::string name, short pinA, short pinB, short steps = 4)
        : Module(std::move(name)), pinA(pinA), pinB(pinB), steps(steps) {}

    virtual void onEncoderChanged(short delta)
    {
        long value = encoder.readEncoder();
        if (delta && ctx.bus) {
            // say("[Encoder changed] value: %d, delta: %d", value, delta);
            // When a small motor is affixed to the dial, a haptic feedback response should be requested
            //ctx.bus->publish({Event::HapticFeedbackRequest, this, delta > 0 ? 0 : 180}); // haptic 0 (degrees) for forward, 180 (degrees) for reverse direction
            if (delta < 0) ctx.bus->publish({Event::EncoderCCW, this, -delta});
            if (delta > 0) ctx.bus->publish({Event::EncoderCW, this, delta});
            ctx.bus->publish({Event::EncoderValueChanged, this, value});
        }
    }
    inline AiEsp32RotaryEncoder getEncoder() { return encoder; }

protected:
    void setupEncoderISR();

    bool setup() override
    {
        encoder.begin();
        setupEncoderISR();

        encoder.setBoundaries(0, STEP_COUNT-1, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        encoder.setAcceleration(STEP_COUNT / 12);
        encoder.setEncoderValue(0);

        // The following two properties make the pin mode INPUT_PULLUP (defaults to INPUT_PULLDOWN).
        // This works as long as the GPIO pin is read/write. In the case where it is read-only
        // then a 10k resistor must be used to pull up the three pins.
        encoder.isButtonPulldown = false;
        encoder.areEncoderPinsPulldownforEsp32 = false;

        return true;
    }

    void cycle() override
    {
        long delta = encoder.encoderChanged();
        if (delta) onEncoderChanged((short)delta);
    }

private:
    short pinA;
    short pinB;
    short steps;

    AiEsp32RotaryEncoder encoder = AiEsp32RotaryEncoder(pinA, pinB, -1, -1, steps);
};

static AiEsp32RotaryEncoder* encoderPinMap[40] = {nullptr};
static void IRAM_ATTR readEncoderISR()
{
    // TODO: below, 39 should be pinA (or pinB if pinA is null)
    encoderPinMap[39]->readEncoder_ISR();
}

inline void EncoderModule::setupEncoderISR()
{
    encoderPinMap[pinA] = &encoder;
    encoderPinMap[pinB] = &encoder;
    encoder.setup(readEncoderISR);
}
