#include "module/ModuleRegistry.h"

#include <Wire.h>

#include "Module.h"
#include "say.h"
#include "systemSetup.h"

ModuleRegistry::ModuleRegistry()
{
    modules.reserve(INITIAL_MODULES);
}

[[nodiscard]] ModuleRegistry& ModuleRegistry::instance()
{
    static ModuleRegistry r;
    return r;
}

void ModuleRegistry::registerModule(Module& module)
{
    say("[ModuleRegistry] Registering: %s", module.getName());
    modules.push_back(&module);
}

void ModuleRegistry::unRegisterModule(Module& module)
{
    // std::erase(modules, &module);
    say("[ModuleRegistry] UnRegistered: %s", module.getName());
}

void ModuleRegistry::preSetupAll()
{

    // say("[ModuleRegistry] Pre-setup. needsWireInitialization = %d", needsWireInitialization);
    if (needsWireInitialization)
    {
        Wire.begin(I2C_SDA, I2C_SCL, I2C_BUS_FREQUENCY);
    }
    // say("[ModuleRegistry] Pre-setup. needsVSPIInitialization = %d", needsVSPIInitialization);
    if (needsVSPIInitialization)
    {
        // ctx.vspi.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, -1);
    }
    // say("[ModuleRegistry] Pre-setup. needsHSPIInitialization = %d", needsHSPIInitialization);
    if (needsHSPIInitialization)
    {
        // ctx.hspi = SPIClass(HSPI);
        // hSPI.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, -1);
    }
}

void ModuleRegistry::setupAll()
{
    preSetupAll();

    say("[ModuleRegistry] Setting up %d modules...", modules.size());
    for (auto module : modules) {
        if (module)
        {
            say("  -> %s (%p)", module->getName() ? module->getName() : "UNKNOWN", module->getName());
            if (!module->moduleSetup())
                unRegisterModule(*module);
        } else
        {
            say("Empty module");
        }
    }
    modules.shrink_to_fit();
}

void ModuleRegistry::cycleAll()
{
    for (auto module : modules) {
        module->moduleCycle();
    }
    delay(50);
}
