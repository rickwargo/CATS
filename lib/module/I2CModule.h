#pragma once

#include "Module.h"

#include <utility>

struct I2CConfig {
};

class I2CModule : public Module {
public:
    I2CModule(std::string name, unsigned long cycleCheckTime = DEFAULT_CYCLE_CHECK_MS) : Module(std::move(name), cycleCheckTime) {}
};
