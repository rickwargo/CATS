#pragma once

#include "Property.h"

class NumericMeasure: public FaceModule
{
public:
    NumericMeasure(std::string name, unsigned long cycleCheckTime, const char *measureUnit = nullptr)
        : FaceModule(std::move(name), cycleCheckTime), measureUnit(measureUnit) {}

protected:
    const char *measureUnit;
    std::vector<float> measures;
    Property<float, NumericMeasure> measure{this, &NumericMeasure::getFirstMeasure, &NumericMeasure::setFirstMeasure};
    const char *displayFmt = "---";  // must stay within the confines of available letters in the font
    void setMeasureValue(float value) { measures = {value}; displayFmt = "%.1f"; }
    void setMeasureValue(double value) { measures = {(float)value}; displayFmt = "%.1f"; }
    void setMeasureValue(int value) { measures = {(float)value}; displayFmt = "%d"; }
    void setMeasures(std::vector<float> values) { measures = values; displayFmt = "%.1f"; }

    void renderToSprite(TFT_eSprite& sprite)
    {
        char buf[20];
        snprintf(buf, sizeof buf, measureUnit ? "%s (%s)" : "%s", getName(), measureUnit);
        sprite.setTextFont(2);
        sprite.setTextColor(FACE_FOREGROUND_COLOR_DEFAULT);
        sprite.drawString(buf, FACE_CENTER_X, FACE_SPRITE_W * 1 / 4);

        sprite.setTextColor(TEXT_FOREGROUND_COLOR_DEFAULT);
        sprite.setTextSize(1);
        sprite.setTextFont(measures.size() == 1 ? 6 : 4);
        int y = FACE_CENTER_Y - (measures.size() - 1) * 16;
        for (auto m : measures)
        {
            snprintf(buf, sizeof buf, displayFmt, displayFmt == "%d" ? (int)m : (float)m);
            sprite.drawString(buf, FACE_CENTER_X, y);
            y += 32;
        }
    }

    void renderFace() override
    {
        renderToSprite(*ctx.sprite);
    }

private:
    [[nodiscard]] float getFirstMeasure() const { return !measures.empty() ? measures[0] : 0; }
    void setFirstMeasure(const float value) { measures = {value}; displayFmt = "%.1f"; }
};
