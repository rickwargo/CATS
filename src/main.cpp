#include <Arduino.h>
#include "ModuleRegistry.h"

void setup() {
    extern void loadModules();
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("\n\n================================");
    Serial.println("ESP32 IoT System - Initializing");
    Serial.println("================================\n");


    loadModules();
    ModuleRegistry::instance().setupAll();

    Serial.println("\nSetup complete! System ready.\n");
}

void loop()
{
    ModuleRegistry::instance().cycleAll();
}
