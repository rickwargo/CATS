#pragma once

#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "car.h"


class AutoFace : public FaceModule, ISubscriber
{
public:
    AutoFace(const char* name = "Auto Direction", unsigned long cycleCheckTime = 100) : FaceModule(name, cycleCheckTime) {}

    void onEvent(const Event& e) override
    {
        // say("[onEvent %p] %s, data = %d", e.source, e.getName(), e.data);
        bool needsToRender = false;
        if (e.type == Event::YawChanged && e.data != yaw)
        {
            yaw = e.data;
            needsToRender = true;
        }
        else if (e.type == Event::PitchChanged && e.data != pitch)
        {
            pitch = e.data;
            needsToRender = true;
        }
        else if (e.type == Event::RollChanged && e.data != roll)
        {
            roll = e.data;
            needsToRender = true;
        }
        else if (e.type == Event::ButtonClick)
        {
            speed = (speed + 7) % 100;
            needsToRender = true;
        }
        else if (e.type == Event::ButtonLong)
        {
            speed = random(100);
            needsToRender = true;
        }
        if (needsToRender && isActiveFace())
        {
            drawFace();
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);
        disableWindowChrome();

        return true;
    }

    virtual void onActivate()
    {
        offscreen.setColorDepth(COLOR_DEPTH_DEFAULT);
        offscreen.createSprite(TFT_WIDTH, TFT_HEIGHT);
        offscreen.setPivot(FACE_CENTER_X, FACE_CENTER_Y);
        offscreen.setTextDatum(TEXT_DATUM_DEFAULT);
        offscreen.setTextColor(TFT_BLACK, TFT_DARKGREY);
        offscreen.setTextFont(4);
        offscreen.fillSprite(GROUND_COLOR);
        framePercentage = 0;
        drawRoad(offscreen, 60, framePercentage);
        drawCar(offscreen, -999, 60, true);

        drawFace();
    }

    virtual void onDeactivate()
    {
        offscreen.deleteSprite();
    }

    void renderFace() override
    {
        offscreen.pushRotated(ctx.sprite, yaw, TFT_TRANSPARENT);
    }

    void cycle() override
    {
        framePercentage = (framePercentage + speed) % 100;
        drawRoad(offscreen, 60, framePercentage);
        drawCar(offscreen, -999, 60, true);

        char buf[8];
        snprintf(buf, sizeof(buf), "%d mph", speed);
        offscreen.drawString(buf, FACE_CENTER_X, TFT_HEIGHT - 16);

        drawFace();
    }

private:
    short yaw = 0;
    short pitch = 0;
    short roll = 0;
    short speed = 0;
    TFT_eSprite offscreen = TFT_eSprite(ctx.display);
    short framePercentage = 0;
};
