#pragma once

#include <Arduino.h>
#include <TimeLib.h>
#include <Timezone.h>
#include "FaceModule.h"

#define CLOCK_FG   FACE_FOREGROUND_COLOR_DEFAULT
#define CLOCK_BG   FACE_BACKGROUND_COLOR_DEFAULT              //  faceBgColors[FACE_CLOCK]
#define SECOND_FG  TFT_RED
#define LABEL_FG   TFT_GOLD

#define CLOCK_R       (TFT_WIDTH / 2.0f) // Clock face radius (float type)
#define H_HAND_LENGTH (CLOCK_R/2.0f)
#define M_HAND_LENGTH (CLOCK_R/1.4f)
#define S_HAND_LENGTH (CLOCK_R/1.3f)

#define CLOCK_CENTER_X CLOCK_R
#define CLOCK_CENTER_Y CLOCK_R

#define CLOCK_BORDER 4

// Calculate 1-second increment angles. Hours and minute hand angles
// change every second so we see smooth sub-pixel movement
#define SECOND_ANGLE (360.0 / 60.0)
#define MINUTE_ANGLE (SECOND_ANGLE / 60.0)
#define HOUR_ANGLE   (MINUTE_ANGLE / 12.0)

class ClockFace : public FaceModule
{
public:
    ClockFace(const char* name, unsigned long cycleCheckTime) : FaceModule(std::move(name), cycleCheckTime) {}

protected:
    bool setup() override
    {
        ctx.sprite->setTextDatum(MC_DATUM);    // Set text datum to middle center and the color

        return true;
    }

    void renderFace() override
    {
        renderClock(now());
    }

private:
    time_t toLocalTime(const time_t utc)
    {
        constexpr TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // UTC - 4 hours
        constexpr TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // UTC - 5 hours

        Timezone usEastern(usEDT, usEST);
        return usEastern.toLocal(utc);
    }
    // =========================================================================
    // Draw the clock face in the sprite
    // The face is completely redrawn - this can be done quickly
    // =========================================================================
    void renderClock(time_t tm)
    {
        tm = toLocalTime(tm);

        const double h_angle = (tm % 86400) * HOUR_ANGLE;
        const double m_angle = (tm % 86400) * MINUTE_ANGLE;
        const double s_angle = (tm % 86400) * SECOND_ANGLE;

        // The background color will be used during the character rendering, and numbers on dial should have same color ticks
        ctx.sprite->setTextColor(CLOCK_FG, CLOCK_BG);
        ctx.sprite->setTextFont(1);
        ctx.sprite->setTextSize(2);

        // Text offset adjustment
        constexpr uint32_t dialOffset = CLOCK_R - 18;

        float xp = 0.0, yp = 0.0; // Use float pixel position for smooth AA motion

        // Draw digits around clock perimeter
        for (uint32_t h = 1; h <= 12; h++)
        {
            getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &xp, &yp, dialOffset, h * 360.0 / 12);
            if (h % 3 == 0)
                ctx.sprite->drawNumber(h, xp + (h == 2 ? 2 : 0), 4 + yp - (h == 6 ? 4 : 0));
            else
                drawDash(h);
        }

        // Draw the day and date
        ctx.sprite->setTextColor(TEXT_FOREGROUND_COLOR_DEFAULT, TFT_BLACK);
        ctx.sprite->setTextFont(2);
        ctx.sprite->setTextSize(1);
        char buf[10];
        snprintf(buf, sizeof(buf), "%-3.3s %2d", dayShortStr(dayOfWeek(tm)), day(tm));
        buf[1] = toupper(buf[1]);
        buf[2] = toupper(buf[2]);
        ctx.sprite->drawString(buf, FACE_SPRITE_W * 3 / 4, FACE_CENTER_Y+3); // to align more with the number "3" than the center

        // Draw minute hand
        getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &xp, &yp, M_HAND_LENGTH, m_angle);
        ctx.sprite->drawWideLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, xp, yp, CLOCK_R / 12.0f, CLOCK_FG);
        ctx.sprite->drawWideLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, xp, yp, CLOCK_R / 12.0f / 3.0f, CLOCK_BG);

        // Draw hour hand
        getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &xp, &yp, H_HAND_LENGTH, h_angle);
        ctx.sprite->drawWideLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, xp, yp, CLOCK_R / 12.0f, CLOCK_FG);
        ctx.sprite->drawWideLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, xp, yp, CLOCK_R / 12.0f / 3.0f, CLOCK_BG);

        // Draw the central pivot circle
        ctx.sprite->fillSmoothCircle(CLOCK_R, CLOCK_R, CLOCK_R / 16, TFT_MAROON);

        // Draw second hand
        getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &xp, &yp, S_HAND_LENGTH, s_angle);
        ctx.sprite->drawWedgeLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, xp, yp, CLOCK_R / 64.0f * 1.25f, 1.0f, SECOND_FG);
    }

    void drawBand(int centerX, int centerY, int thickness, unsigned short fgColor, unsigned short bgColor)
    {
        for (int radius = thickness; radius > 0; radius--)
        {
            ctx.sprite->drawSmoothCircle(centerX, centerY, CLOCK_R - thickness + radius, fgColor, bgColor);
        }
    }

    void drawBand(int centerX, int centerY, int thickness, unsigned short fgColor)
    {
        drawBand(centerX, centerY, thickness, fgColor, fgColor);
    }

    void drawDash(uint32_t h)
    {
        float x1, y1, x2, y2;
        getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &x1, &y1, CLOCK_R - 5, h * 360.0 / 12);
        getCoordClock(CLOCK_CENTER_X, CLOCK_CENTER_Y, &x2, &y2, CLOCK_R - 12, h * 360.0 / 12);
        ctx.sprite->drawLine(x1, y1, x2, y2, CLOCK_FG);
        ctx.sprite->drawLine(x1 + 1, y1, x2 + 1, y2, CLOCK_FG);
        ctx.sprite->drawLine(x1 + 2, y1, x2 + 2, y2, CLOCK_FG);
    }

    // =========================================================================
    // Get coordinates of end of a line, pivot at x,y, length r, angle a
    // =========================================================================
    // Coordinates are returned to caller via the xp and yp pointers
#define DEG2RAD 0.0174532925

    void getCoordClock(short x, short y, float* xp, float* yp, short r, float a)
    {
        float sx1 = cos((a - 90) * DEG2RAD);
        float sy1 = sin((a - 90) * DEG2RAD);
        *xp = sx1 * r + x;
        *yp = sy1 * r + y;
    }
};
