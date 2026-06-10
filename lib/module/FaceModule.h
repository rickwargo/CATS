#pragma once

#include "Module.h"
#include "module/DisplayModule.h"

// Sprite width and height
#define FACE_RADIUS                 (TFT_WIDTH/2)
#define FACE_CENTER_X               (TFT_WIDTH/2)
#define FACE_CENTER_Y               (TFT_HEIGHT/2)

class FaceModule : public Module
{
public:
    FaceModule(std::string name, unsigned long cycleCheckTime) : Module(std::move(name), cycleCheckTime) {}
    virtual bool ignoreEvents()
    {
        return  !requiresBackgroundProcessing() && !isActiveFace();
    }
    virtual void onActivate() { drawFace(); }
    virtual void onDeactivate() {}

protected:
    unsigned int faceBgColor = FACE_BACKGROUND_COLOR_DEFAULT;
    unsigned int faceFgColor = FACE_FOREGROUND_COLOR_DEFAULT;
    unsigned int textBgColor = TFT_TRANSPARENT;
    unsigned int textFgColor = TEXT_FOREGROUND_COLOR_DEFAULT;
    unsigned short textDatum = TEXT_DATUM_DEFAULT;

    bool isActiveFace()
    {
        return ctx.activeFace && ctx.activeFace == this;
    }

    bool preSetup() override
    {
        return true;
    }

    bool canCycle() override
    {
        return isActiveFace() || requiresBackgroundProcessing();
    }

    virtual void drawWindowChrome()
    {
        for (int initialBorder = 4, border = initialBorder; border > 0; border--)
        {
            ctx.sprite->drawSmoothCircle(FACE_CENTER_X, FACE_CENTER_Y,FACE_RADIUS - initialBorder + border, faceFgColor, faceFgColor);
        }
    }

    virtual void renderFace() {}

    void drawFace(unsigned int bgColor = FACE_BACKGROUND_COLOR_DEFAULT)
    {
        // Clear the sprite
        ctx.sprite->fillSprite(bgColor);

        renderFace();
        drawWindowChrome();
        ctx.sprite->pushSprite(0, 0);
    }

    void cycle() override
    {
        if (isActiveFace())
        {
            // say("[cycle] Drawing due to cycle");
            drawFace();
        }
    }
};
