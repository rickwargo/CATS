#include "module/Module.h"

#include <Arduino.h>
#include <TimeLib.h>

#include "say.h"

bool Module::moduleSetup()
{
    initialized = false;
    try
    {
        if (!canSetup()) return false;
        if (!preSetup()) return false;
        if (!setup()) return false;

        initialized = true;
        return true;
    }
    catch (std::exception& e)
    {
        say("[%s] ERROR: Exception during setup", getName());
    }
    return false;
}

bool Module::timeToCycle()
{
    if (!canCycle()) return false;
    if (millis() < targetTime) return false;

    targetTime = millis() + cycleCheckTime;
    return true;
}

void Module::moduleCycle()
{
    if (!timeToCycle()) return;

    if (!initialized)
    {
        if (reInitCount < 0)
        {
            say("[Module ERROR] %s is NOT initialized", getName());
            reInitCount++;  // To end the slew of first time error messages
            reInitTime = now() + reInitDelay; // Retry after X seconds
        }

        if (now() >= reInitTime)
        {
            if (++reInitCount > 0)
                say("[%s] attempt #%d to re-initialize module", getName(), reInitCount);

            if (this->moduleSetup() || setup())
            {
                say("[%s] Successful re-initialization on attempt #%d", getName(), reInitCount);
                initialized = true;
                reInitCount = -1; // Reset count
                reInitDelay = 10; // Reset delay
            }
            else
            {
                if (reInitCount % 5 == 0) reInitDelay *= 2; // Exponential backoff
                if (reInitDelay > 900) reInitDelay = 900; // Cap at 15 minutes
                reInitTime = now() + reInitDelay; // Retry after X seconds
            }
        }
    }
    else
    {
        try
        {
            preCycle();
            cycle();
        }
        catch (std::exception& e)
        {
            // Handle exception
            // if too many occur, direct the module's cycle towards longer dormancies
            say("[%s]: Exception during cycle: %s", getName(), e.what());
        }
    }
}

void Module::tryToRecoverFromError()
{
    // This is called after a successful initialization and working, but some problem necessitates initializing again
    initialized = false;
}
