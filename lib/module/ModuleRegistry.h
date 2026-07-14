#pragma once

#include <vector>
#include <memory>
#include <Processors/TFT_eSPI_ESP32_S3.h>

#include "systemSetup.h"
#if defined(VSPI_CLK) || defined(HSPI_CLK)
#include <SPI.h>
#endif

class Module;

/**
 * ModuleRegistry - Central management system for all modules
 *
 * Responsible for:
 * - Registering/unregistering modules
 * - Calling setup() on all modules during initialization
 * - Calling cycle() on all modules during main loop
 * - Tracking module count and status
 */
class ModuleRegistry {
public:
    std::vector<Module*> modules;
    bool needsWireInitialization = true;
    bool needsVSPIInitialization = false;
    bool needsHSPIInitialization = false;
#ifdef VSPI_CLK
    // say("[ModuleRegistry] Pre-setup. needsVSPIInitialization = %d", needsVSPIInitialization);
    SPIClass vSPI = SPIClass(VSPI);
#endif
#ifdef HSPI_CLK
    // say("[ModuleRegistry] Pre-setup. needsHSPIInitialization = %d", needsHSPIInitialization);
    SPIClass hSPI = SPIClass(HSPI);
#endif

    ModuleRegistry();
    static ModuleRegistry& instance();
    void registerModule(Module& module);
    void unRegisterModule(Module& module);
    void setupAll();
    void cycleAll();
    bool initializeHardware();

private:
    static constexpr int INITIAL_MODULES = 32;
};
