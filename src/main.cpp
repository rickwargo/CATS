#include <Arduino.h>
#include <Wire.h>
#include "ModuleRegistry.h"

void setup() {
    extern void loadModules();

    Serial.begin(115200);
    while (!Serial) {}
    Serial.println(F("[CATS] Starting up... "));

#if CORE_DEBUG_LEVEL > 4
    // Serial.println(F("[CATS] Sleeping for 5 seconds to give serial monitor a chance to connect... "));
    // delay(5000);    // give a chance for the serial monitor to display

    Serial.println(F("\n================================"));
    Serial.println(F("ESP32 IoT System - Initializing"));
    Serial.println(F("================================\n"));
#endif

    if (!ModuleRegistry::instance().initializeHardware())
    {
        Serial.println(F("[ERROR] Problems while trying to initialize the hardware. Aborting startup."));
        while (1) ;
    }

    loadModules();

    ModuleRegistry::instance().setupAll();

    Serial.println(F("\n[CATS] Setup complete! System ready.\n"));
}

void loop()
{
    ModuleRegistry::instance().cycleAll();
}
