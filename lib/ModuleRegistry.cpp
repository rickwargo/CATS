#include "ModuleRegistry.h"

#include <HardwareSerial.h>
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
    std::erase(modules, &module);
    say("[ModuleRegistry] UnRegistered: %s", module.getName());
}

bool ModuleRegistry::initializeHardware()
{
#ifdef I2C_SDA
    // say("[ModuleRegistry] Pre-setup. needsWireInitialization = %d", needsWireInitialization);
    if (needsWireInitialization)
    {
        if (!Wire.begin(I2C_SDA, I2C_SCL, I2C_BUS_FREQUENCY))
        {
            Serial.println("[ERROR] Could not initialize I2C.");
            return false;
        }
    }
#endif
#ifdef VSPI_CLK
    // say("[ModuleRegistry] Pre-setup. needsVSPIInitialization = %d", needsVSPIInitialization);

    if (needsVSPIInitialization)
    {
        // Caller must make sure to set the Slave Select pin as an output and
        // keep it HIGH (deselected) for all SPI device during initialization

        // Initialize VSPI with specific pins
        if (!vSPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, -1))
        {
            Serial.println("[ERROR] Could not initialize VSPI.");
            return false;
        }
    }
#endif
#ifdef HSPI_CLK
    // say("[ModuleRegistry] Pre-setup. needsHSPIInitialization = %d", needsHSPIInitialization);
    if (needsHSPIInitialization)
    {
        // Caller must make sure to set the Slave Select pin as an output and
        // keep it HIGH (deselected) for any SPI device during initialization

        // Initialize HSPI with specific pins
        if (!hSPI.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, -1))
        {
            Serial.println("[ERROR] Could not initialize HSPI.");
            return false;
        }
    }
#endif
    return true;
}

void ModuleRegistry::setupAll()
{
    say("[ModuleRegistry] Setting up %d modules...", modules.size());
    for (auto module : modules) {
        if (module)
        {
            say("  -> %s (%p)", module->getName() ? module->getName() : "UNKNOWN", module->getName());
            if (!module->moduleSetup())
                unRegisterModule(*module);  // This should only be done if it can not be initialized again
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
