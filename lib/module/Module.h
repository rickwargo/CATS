#pragma once
#include <utility>
#include <memory>

#include "EventBus.h"
#include "DependencyContext.h"

inline EventBus bus;
inline DependencyContext appContext{&bus};

class Module {
public:
    virtual ~Module() = default; // Makes the object polymorphic
    static constexpr int DEFAULT_CYCLE_CHECK_MS = 50;

    Module(std::string name, unsigned long cycleCheckTime = DEFAULT_CYCLE_CHECK_MS)
        : moduleName(std::move(name)), cycleCheckTime(cycleCheckTime) {}

    [[nodiscard]] const char* getName() const { return !moduleName.empty() ? moduleName.c_str() : "UNDEFINED"; }
    void setContext(DependencyContext& _ctx) { ctx = _ctx; }

    bool moduleSetup();
    void moduleCycle();

protected:
    DependencyContext& ctx = appContext;
    bool backgroundProcessing = false;

    virtual bool canSetup() { return true; }
    virtual bool preSetup() { return true; }
    virtual bool setup() { return true; }
    virtual bool timeToCycle();
    virtual bool canCycle() { return true; }
    virtual void preCycle() {}
    virtual void cycle() {}

    virtual void tryToRecoverFromError();

    void setCycleCheckTime(unsigned long _cycleCheckTime) { this->cycleCheckTime = _cycleCheckTime; }
    [[nodiscard]] bool requiresBackgroundProcessing() const
    {
        return backgroundProcessing;
    }

private:
    bool initialized = false;
    unsigned long cycleCheckTime;
    unsigned long targetTime = 0; // Time for next action check
    std::string moduleName;
    time_t reInitTime = 0; // Time to attempt reinitialization if initialization fails
    int reInitCount = -1;
    int reInitDelay = 10; // seconds
};
