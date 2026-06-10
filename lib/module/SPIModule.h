#pragma once

#include "Module.h"

// struct SPIConfig {
// };

class SPIModule : public Module {
public:
    SPIModule(std::string name) : Module(std::move(name)) {}
};
