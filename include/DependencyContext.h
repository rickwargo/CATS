#pragma once

#include "EventBus.h"

class TFT_eSPI;
class TFT_eSprite;
class FaceModule;

struct DependencyContext {
    EventBus* bus = nullptr;
    TFT_eSPI* display = nullptr;
    TFT_eSprite* sprite = nullptr;
    FaceModule* activeFace = nullptr;
};
