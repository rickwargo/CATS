#include "systemSetup.h"
#include "ModuleRegistry.h"

#include "module/SystemModule.h"
#include "module/DisplayModule.h"
#include "module/NetworkModule.h"
#include "module/TimeModule.h"
#include "module/ButtonModule.h"
#include "module/BuzzerModule.h"
#include "module/EncoderModule.h"
#include "module/LightLevelModule.h"
// #include "module/HapticModule.h"
// #include "module/ShifterModule.h"
#include "module/EnvironmentModule.h"
#include "module/IMUModule.h"
// #include "module/StorageModule.h"
#include "face/FaceManager.h"
#include "face/ClockFace.h"
#include "face/TemperatureFace.h"
#include "face/HumidityFace.h"
#include "face/IAQFace.h"
#include "face/PressureFace.h"
#include "face/IMUFace.h"
#include "face/LightLevelFace.h"
#include "face/MeasuresFace.h"
// #include "face/DGFace.h"

void loadModules()
{
    ModuleRegistry& registry = ModuleRegistry::instance();
    // Load order is significant - specify dependent modules first
    static SystemModule systemModule("System Handler");
    static DisplayModule displayModule("Round Display");
    static NetworkModule networkModule("Network Overlord");
    static TimeModule timeModule("Time Syncer");
    static ButtonModule buttonModule("Rotary Button", ROTARY_BUTTON_PIN);
    static BuzzerModule buzzerModule("Main Buzzer", BUZZER_PIN);
    static EncoderModule encoderModule("Rotary Encoder", ENCODER_A, ENCODER_B, ENCODER_DEFAULT_STEPS);
    static LightLevelModule lightLevelModule("Light Level Sensor", 2000);
    // static HapticModule hapticForwardModule("Haptic Feedback Forward", HAPTIC_PIN_1);
    // static HapticModule hapticReverseModule("Haptic Feedback Reverse", HAPTIC_PIN_2);
    // static ShifterModule shifterModule("Shifter LED", SHIFTER_PIN);
    static EnvironmentModule environmentModule("Environment Sensor", 1000);
    static IMUModule imuModule("Inertial Measurement Unit", 100);
    // static StorageModule storageModule("SD Card Storage");

    static FaceManager faceManager("Face Manager", 1000);   // Must be loaded prior to loading faces
    static ClockFace clockFace("Clock", 500);
    static TemperatureFace temperatureFace("Inside Temperature");
    static HumidityFace humidityFace("Humidity");
    static IAQFace iaqFace("IAQ");
    static PressureFace pressureFace("Pressure");
    static LightLevelFace lightLevelFace("Light Level");
    static IMUFace imuFace("IMU");
    static MeasuresFace measuresFace("Measures");
    // static DGFace dgFace("DG", 100);

    registry.registerModule(systemModule);
    registry.registerModule(displayModule);
    registry.registerModule(networkModule);
    registry.registerModule(timeModule);
    registry.registerModule(buttonModule);
    registry.registerModule(buzzerModule);
    registry.registerModule(encoderModule);
    registry.registerModule(lightLevelModule);
    // registry.registerModule(hapticForwardModule);
    // registry.registerModule(hapticReverseModule);
    // registry.registerModule(shifterModule);
    registry.registerModule(environmentModule);
    registry.registerModule(imuModule);
    // registry.registerModule(storageModule);
    registry.registerModule(faceManager);
    registry.registerModule(clockFace);
    registry.registerModule(temperatureFace);
    registry.registerModule(humidityFace);
    registry.registerModule(iaqFace);
    registry.registerModule(pressureFace);
    registry.registerModule(lightLevelFace);
    registry.registerModule(imuFace);
    registry.registerModule(measuresFace);
    // registry.registerModule(dgFace);
}
