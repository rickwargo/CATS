#pragma once

#include <vector>
#include <memory>

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
    ModuleRegistry();

    static ModuleRegistry& instance();
    void registerModule(Module& module);
    void unRegisterModule(Module& module);
    void setupAll();
    void cycleAll();

    std::vector<Module*> modules;
    bool needsWireInitialization = true;
    bool needsVSPIInitialization = false;
    bool needsHSPIInitialization = false;

private:
    static constexpr int INITIAL_MODULES = 32;
    void preSetupAll();
};
