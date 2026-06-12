#pragma once

#include "car.h"
#include "FaceModule.h"
#include "ISubscriber.h"
#include "Event.h"


class DGFace : public FaceModule, ISubscriber
{
public:
    DGFace(const char* name = "Directional Gyro", unsigned long cycleCheckTime = 100) : FaceModule(name, cycleCheckTime) {}

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
        offscreen.setTextColor(TFT_GOLD, TFT_TRANSPARENT);

        drawFace();
    }

    virtual void onDeactivate()
    {
        offscreen.deleteSprite();
    }

    void renderFace() override
    {
        drawHorizonBackground(offscreen, roll);
        offscreen.pushRotated(ctx.sprite, pitch);
        drawCompassRoseSprite(offscreen);
        offscreen.pushRotated(ctx.sprite, -yaw, TFT_TRANSPARENT);
        drawDirectionalSymbol(*ctx.sprite);
    }

    void cycle() override {}

private:
    short yaw = 0;
    short pitch = 0;
    short roll = 0;
    TFT_eSprite offscreen = TFT_eSprite(ctx.display);

    void drawCompassRoseSprite(TFT_eSprite& sprite)
    {
        sprite.fillSprite(TFT_TRANSPARENT);
        sprite.setTextFont(2);
        sprite.setTextSize(1);

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

            sprite.drawLine(x0, y0, x1, y1, TFT_CYAN);
        }

        // -------- 30° Numbers (Except N/E/S/W) --------
        for (int deg = 0; deg < 360; deg += 30)
        {
            // Skip cardinal directions
            if (deg % 90 == 0)
                continue;

            float angle = radians(deg);
            int x = FACE_CENTER_X + sin(angle) * 72;
            int y = FACE_CENTER_Y - cos(angle) * 72;

            sprite.drawNumber(deg, x, y); // 30→3, 60→6, etc.
        }

        // -------- N E S W Letters --------
        const char *cardinal[4] = {"N", "E", "S", "W"};

        sprite.setTextFont(1);
        sprite.setTextSize(2);
        for (int i = 0; i < 4; i++)
        {
            float angle = radians(i * 90);
            int x = FACE_CENTER_X + sin(angle) * 70;
            int y = FACE_CENTER_Y - cos(angle) * 70;
            sprite.drawString(cardinal[i], x, y);
        }
    }

    int nose = -58;
    void drawDirectionalSymbol(TFT_eSprite& sprite) const
    {
        drawCar(sprite, nose, 15);
        // drawDirectionalSymbolTriangle(sprite);
    }

    void drawDirectionalSymbolTriangle(TFT_eSprite& sprite) const
    {
        // =================================================
        // Directional Symbol (Centered, Larger)
        // =================================================

        int cx = FACE_CENTER_X;
        int cy = FACE_CENTER_Y + nose;

        // Nose (extended upward)
        sprite.fillTriangle(
            cx, cy,
            cx - 16 / 2, cy + 20,
            cx + 16 / 2, cy + 20,
            TFT_GREEN
        );

    }

    void drawHorizonBackground(TFT_eSprite& sprite, short rollAngle)
    {
        int pitchLineThickness = 5;
        int h = FACE_CENTER_Y - (TFT_HEIGHT / 2 * rollAngle / 90);
        sprite.fillRect(0, 0, TFT_WIDTH, h, SKY_COLOR);
        sprite.fillRect(0, h, TFT_WIDTH, TFT_HEIGHT - h, GROUND_COLOR);
        sprite.fillRect(0, h - (pitchLineThickness - 1)/2, TFT_WIDTH, pitchLineThickness, TFT_RED);
    }
};
