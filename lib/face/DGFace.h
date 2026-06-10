#pragma once

#include "module/FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"


class DGFace : public FaceModule, ISubscriber
{
public:
    DGFace(const char* name, unsigned long cycleCheckTime) : FaceModule(std::move(name), cycleCheckTime) {}

    void onEvent(const Event& e) override
    {
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
        if (needsToRender && isActiveFace())
        {
            say("[DGFace.onEvent] Drawing due to event");
            drawFace();
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);
        initializeCompassRoseSprite();
        compassRose.setPivot(FACE_CENTER_X, FACE_CENTER_Y);
        horizon.setPivot(FACE_CENTER_X, FACE_CENTER_Y);


        return true;
    }

    void renderFace() override
    {
        say("[DGFace.renderFace] Rendering image");
        drawPitchRoll(pitch, roll);
        compassRose.pushRotated(&*ctx.sprite, yaw < 0 ? yaw + 360 : yaw, TFT_TRANSPARENT);
        horizon.pushRotated(&*ctx.sprite, pitch < 0 ? pitch + 360 : pitch, TFT_TRANSPARENT);
        drawDirectionalSymbol(*ctx.sprite);
    }

    void cycle() override {}

private:
    int16_t yaw = 0;
    int16_t pitch = 0;
    int16_t roll = 0;
    TFT_eSprite compassRose = TFT_eSprite(ctx.display);
    TFT_eSprite horizon = TFT_eSprite(ctx.display);

    void initializeCompassRoseSprite()
    {
        compassRose.fillSprite(TFT_TRANSPARENT);
        compassRose.setTextColor(TFT_GOLD, TFT_TRANSPARENT);
        compassRose.setTextFont(1);

        // -------- Compass Card Ticks --------
        for (int deg = 0; deg < 360; deg += 5)
        {
            float angle = radians(deg);

            int rOuter = 110;
            int rInner = 105;

            if (deg % 30 == 0)
            {
                rInner = 88; // Long tick (30°)
            }
            else if (deg % 10 == 0)
            {
                rInner = 98; // Medium tick (10°)
            }

            int x0 = FACE_CENTER_X + sin(angle) * rInner;
            int y0 = FACE_CENTER_Y - cos(angle) * rInner;
            int x1 = FACE_CENTER_X + sin(angle) * rOuter;
            int y1 = FACE_CENTER_Y - cos(angle) * rOuter;

            compassRose.drawLine(x0, y0, x1, y1, TFT_CYAN);
        }

        // -------- 30° Numbers (Except N/E/S/W) --------
        compassRose.setTextSize(2);
        for (int deg = 0; deg < 360; deg += 30)
        {
            // Skip cardinal directions
            if (deg % 90 == 0)
                continue;

            float angle = radians(deg);
            int x = FACE_CENTER_X + sin(angle) * 72;
            int y = FACE_CENTER_Y - cos(angle) * 72;

            compassRose.drawNumber(deg / 10, x, y); // 30→3, 60→6, etc.
        }

        // -------- N E S W Letters --------
        const char *cardinal[4] = {"N", "E", "S", "W"};

        ctx.sprite->setTextSize(3);
        for (int i = 0; i < 4; i++)
        {
            float angle = radians(i * 90);
            int x = FACE_CENTER_X + sin(angle) * 70;
            int y = FACE_CENTER_Y - cos(angle) * 70;
            compassRose.drawString(cardinal[i], x, y);
        }
    }

    void drawDirectionalSymbol(TFT_eSprite& sprite)
    {
        // =================================================
        // Directional Symbol (Centered, Larger)
        // =================================================

        int nose = -58;
        int cx = FACE_CENTER_X;
        int cy = FACE_CENTER_Y + nose;

        // Nose (extended upward)
        sprite.fillTriangle(
            cx, cy,
            cx - 24 / 2, cy + 28,
            cx + 24 / 2, cy + 28,
            TFT_GREEN
        );

    }

    void drawPitchRoll(int16_t pitchAngle, int16_t rollAngle)
    {
        say("[drawPitchRoll] Pitch angle: %d, Roll angle: %d (yaw is %d)", pitchAngle, rollAngle, yaw);

        int h = TFT_HEIGHT * rollAngle / TFT_HEIGHT;
        horizon.fillRect(0, 0, TFT_WIDTH, h, TFT_BLUE);
        horizon.fillRect(0, h, TFT_WIDTH, TFT_HEIGHT - h, TFT_OLIVE);
        horizon.fillRect(0, h - 2, TFT_WIDTH, h + 2, TFT_RED);
    }
};
