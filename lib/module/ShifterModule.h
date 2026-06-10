#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include <utility>
#include "module/PinModule.h"
#include "ISubscriber.h"
#include "Event.h"
#include "systemSetup.h"

#define NUM_LEDS            7
#define BRIGHTNESS          128
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB

class ShifterModule : public PinModule, public ISubscriber
{
public:
    ShifterModule(std::string name, short pin) : PinModule(std::move(name), SHIFTER_PIN) {}

    void onEvent(const Event& e) override
    {
        switch (e.type) {
        case Event::LightLevelDark:
            onLightLevelChange(e);
            break;
        case Event::LightLevelLight:
            onLightLevelChange(e);
            break;
        default:
            break;
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);

        setCycleCheckTime(1000 / SHIFTER_UPDATES_PER_SECOND);

        pinMode(SHIFTER_PIN, OUTPUT);
        FastLED.addLeds<LED_TYPE, SHIFTER_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
        FastLED.setBrightness(  BRIGHTNESS );

        currentPalette = RainbowColors_p;
        currentBlending = LINEARBLEND;

        return true;
    }

    void cycle() override
    {
        ChangePalettePeriodically();

        static unsigned short startIndex = 0;
        startIndex = startIndex + 1; /* motion speed */

        FillLEDsFromPaletteColors(startIndex);

        FastLED.show();

    }

    virtual void onLightLevelChange(const Event& e) {
        switch (e.type)
        {
        case Event::LightLevelDark:
            brightness = 128;
            break;
        case Event::LightLevelLight:
            brightness = 255;
            break;
        default:
            break;
        }
    }

private:
    CRGB leds[NUM_LEDS];
    CRGBPalette16 currentPalette;
    TBlendType    currentBlending;
    unsigned short brightness = BRIGHTNESS;

    void FillLEDsFromPaletteColors(unsigned short colorIndex)
    {
        for( int i = 0; i < NUM_LEDS; ++i) {
            leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
            colorIndex += 3;
        }
    }

    // There are several different palettes of colors demonstrated here.
    //
    // FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
    // OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
    //
    // Additionally, you can manually define your own color palettes, or you can write
    // code that creates color palettes on the fly.  All are shown here.

    void ChangePalettePeriodically()
    {
        unsigned short secondHand = (millis() / 1000) % 60;
        static unsigned short lastSecond = 99;

        if( lastSecond != secondHand) {
            lastSecond = secondHand;
            if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
            if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
            if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
            if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
            if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
            if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
            if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
            if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
            if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
            if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
            if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
        }
    }

    // This function fills the palette with totally random colors.
    void SetupTotallyRandomPalette()
    {
        for( int i = 0; i < 16; ++i) {
            currentPalette[i] = CHSV( random8(), 255, random8());
        }
    }

    // This function sets up a palette of black and white stripes,
    // using code.  Since the palette is effectively an array of
    // sixteen CRGB colors, the various fill_* functions can be used
    // to set them up.
    void SetupBlackAndWhiteStripedPalette()
    {
        // 'black out' all 16 palette entries...
        fill_solid( currentPalette, 16, CRGB::Black);
        // and set every fourth one to white.
        currentPalette[0] = CRGB::White;
        currentPalette[4] = CRGB::White;
        currentPalette[8] = CRGB::White;
        currentPalette[12] = CRGB::White;

    }

    // This function sets up a palette of purple and green stripes.
    void SetupPurpleAndGreenPalette()
    {
        CRGB purple = CHSV( HUE_PURPLE, 255, 255);
        CRGB green  = CHSV( HUE_GREEN, 255, 255);
        CRGB black  = CRGB::Black;

        currentPalette = CRGBPalette16(
                                       green,  green,  black,  black,
                                       purple, purple, black,  black,
                                       green,  green,  black,  black,
                                       purple, purple, black,  black );
    }


    // This example shows how to set up a static color palette
    // which is stored in PROGMEM (flash), which is almost always more
    // plentiful than RAM.  A static PROGMEM palette like this
    // takes up 64 bytes of flash.
    const TProgmemPalette16 myRedWhiteBluePalette_p FL_PROGMEM =
    {
        CRGB::Red,
        CRGB::Gray, // 'white' is too bright compared to red and blue
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::Gray,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Blue,
        CRGB::Black,
        CRGB::Black
    };

};
