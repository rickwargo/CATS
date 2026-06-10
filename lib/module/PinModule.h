#pragma once

#include "Module.h"

struct PinConfig {
    short pin;
    bool pullup;
    bool inverted;
};

class PinModule : public Module {
public:
    PinModule(std::string name, short pin) : Module(std::move(name)), pin(pin) {}

    [[nodiscard]] short getPin() const { return pin; }

protected:
    short pin;
};
