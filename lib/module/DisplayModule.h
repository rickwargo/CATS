#pragma once

#include "systemSetup.h"

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_eSPI_Scroll.h>

#include <utility>
#include "DependencyContext.h"
#include "module/SPIModule.h"

#define FACE_BACKGROUND_COLOR_DEFAULT           TFT_NAVY
#define FACE_FOREGROUND_COLOR_DEFAULT           TFT_CYAN
#define TEXT_BACKGROUND_COLOR_DEFAULT           TFT_NAVY
#define TEXT_FOREGROUND_COLOR_DEFAULT           TFT_GOLD

#ifdef TEXTSCREEN
// #include "4bit.h"
// #define TEXTSCREEN_BACKGROUND_COLOR_DEFAULT     COLOR_BLUE_4B
// #define TEXTSCREEN_FOREGROUND_COLOR_DEFAULT     COLOR_YELLOW_4B
#endif

#define COLOR_DEPTH_DEFAULT                     6
#define TEXT_DATUM_DEFAULT                      MC_DATUM
#define ROTATION_DEFAULT                        0

#define FACE_SPRITE_W                           TFT_HEIGHT
#define FACE_SPRITE_H                           TFT_WIDTH

class DisplayModule : public SPIModule
{
public:
    TFT_eSPI tft;

#ifdef TEXTSCREEN
    enum Mode {
        TextMode,
        GraphicsMode,
    } mode = TextMode;
#endif

    DisplayModule(std::string name) : SPIModule(std::move(name)) {}

#ifdef TEXTSCREEN
    void setMode(const Mode displayMode)
    {
        mode = displayMode;
    }
#endif

    virtual void say(const char* format, ...) {
        char text[128];
        va_list args;
        va_start(args, format);  // Initialize args with the variable arguments
        vsnprintf(text, sizeof(text), format, args);
        va_end(args);
        Serial.println(text);
        Serial.flush();

#ifdef TEXTSCREEN
        if (mode == TextMode)
        {
            textScreen.write(text);
        }
#endif
    }

protected:
#ifdef TEXTSCREEN
    TFT_eSPI_Scroll textScreen;     // Screen to print messages
#endif

    bool setup() override
    {
        tft.init();
        ctx.display = &tft;                // Save this in context as soon as we know it
        ctx.sprite = new TFT_eSprite(&tft);

        tft.setRotation(ROTATION_DEFAULT);

#ifdef TEXTSCREEN
        initializeTextScreen();
#endif
        initializeSprite();

        displayClear();
        tft.setTextFont(2);
        tft.setTextSize(2);
        tft.setTextDatum(TEXT_DATUM_DEFAULT);
        tft.setTextColor(TEXT_FOREGROUND_COLOR_DEFAULT, TEXT_BACKGROUND_COLOR_DEFAULT);
        tft.drawString("Initializing...", TFT_WIDTH / 2, TFT_HEIGHT / 2 - 16);
        tft.drawString("please wait", TFT_WIDTH / 2, TFT_HEIGHT / 2 + 16);

        return true;
    }

    void displayClear()
    {
#ifdef TEXTSCREEN
        tft.setCursor(0, 0);     // Set cursor to top-left
#endif
        tft.fillScreen(FACE_BACKGROUND_COLOR_DEFAULT);
    }

private:
#ifdef TEXTSCREEN
    // void initializeTextScreen()
    // {
    //     if (textScreen.init(&tft, 2, 4) != NO_ERROR) {
    //         Serial.println("[initializeTextScreen] memory allocation failed!");
    //         return;
    //     }
    //     textScreen.setColor(TEXTSCREEN_BACKGROUND_COLOR_DEFAULT, TEXTSCREEN_FOREGROUND_COLOR_DEFAULT);
    // }
#endif
    void initializeSprite()
    {
        ctx.sprite->setColorDepth(COLOR_DEPTH_DEFAULT);
        ctx.sprite->createSprite(FACE_SPRITE_W, FACE_SPRITE_H); // Create the sprite of defined size

        // Set text datum to middle center and the color
        ctx.sprite->setTextDatum(TEXT_DATUM_DEFAULT);
        ctx.sprite->setTextColor(TEXT_FOREGROUND_COLOR_DEFAULT, TEXT_BACKGROUND_COLOR_DEFAULT);
    }

};
