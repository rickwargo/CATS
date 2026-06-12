#pragma once

#include "Measure.h"
#include "Module.h"
#include "module/DisplayModule.h"

// Sprite width and height
#define FACE_RADIUS                 (TFT_WIDTH/2)
#define FACE_CENTER_X               (TFT_WIDTH/2)
#define FACE_CENTER_Y               (TFT_HEIGHT/2)

class FaceModule : public Module
{
public:
    FaceModule(std::string name) : Module(std::move(name)) { setCycleCheckTime(1000); }
    FaceModule(std::string name, unsigned long cycleCheckTime) : Module(std::move(name), cycleCheckTime) {}
    // virtual ~FaceModule() = default;
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

    void renderMeasureToSprite(IMeasure *measure)
    {
        renderMeasuresToSprite({measure}, getName());
    }
    virtual void renderMeasureToSprite(IMeasure *measure, const char *measureName)
    {
        renderMeasuresToSprite({measure}, measureName);
    }
    void renderMeasuresToSprite(const std::vector<IMeasure *>&measures, const char *measureName)
    {
        TFT_eSprite& sprite = *ctx.sprite;
        unsigned int nMeasures = measures.size();
        int measureScreenHeight = 26;
        int y = FACE_CENTER_Y - ((nMeasures - 1) * measureScreenHeight) / 2;
        for (auto m : measures)
        {
            const char* unit = m->unit();
            char buf[42];
            snprintf(buf, sizeof buf, unit && unit[0] ? "%s (%s)" : "%s", measureName, unit);

            sprite.setTextFont(2);
            sprite.setTextColor(FACE_FOREGROUND_COLOR_DEFAULT);
            sprite.drawString(buf, FACE_CENTER_X, FACE_SPRITE_H * 1 / 4);

            sprite.setTextColor(TEXT_FOREGROUND_COLOR_DEFAULT);
            sprite.setTextSize(1);
            sprite.setTextFont(nMeasures > 1 ? 4 : 6);
            m->format(buf, sizeof(buf));
            sprite.drawString(buf, FACE_CENTER_X, y);
            y += measureScreenHeight;
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
            drawFace();
    }
};
