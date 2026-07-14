#pragma once

#if defined(VSPI_CLK) || defined(HSPI_CLK)
#include <SPI.h>
#endif
#include "EventBus.h"

class TFT_eSPI;
class TFT_eSprite;
class FaceModule;

struct DependencyContext {
    EventBus* bus = nullptr;
    TFT_eSPI* display = nullptr;
    TFT_eSprite* sprite = nullptr;
    FaceModule* activeFace = nullptr;
#ifdef VSPI_CLK
    SPIClass* vspi = nullptr;
#endif
#ifdef HSPI_CLK
    SPIClass* hspi = nullptr;
#endif
};
