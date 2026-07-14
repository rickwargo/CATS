#include "systemSetup.h"
#include "ModuleRegistry.h"

#include "SystemModule.h"
#ifdef UI_DISPLAY
#include "DisplayModule.h"
#endif
#ifdef NETWORKING
#include "NetworkModule.h"
#if defined(NETWORK_TIME)
#include "TimeModule.h"
#endif
#endif
#ifdef CANBUS
#include "CANBusModule.h"
#endif
#ifdef BUTTON
#include "ButtonModule.h"
#endif
#ifdef BUZZER_SOUND
#include "BuzzerModule.h"
#endif
#ifdef ENCODER
#include "EncoderModule.h"
#endif
#ifdef LIGHT_LEVEL_SENSOR
#include "LightLevelModule.h"
#endif
#if defined(SHIFTER_KNOB) && defined(HAPTIC)
#include "HapticModule.h"
#endif
#if defined(SHIFTER_KNOB) && (defined(HAPTIC) || defined(FAST_LED))
#include "ShifterModule.h"
#endif
#ifdef ENVIRONMENT_SENSOR
#include "EnvironmentModule.h"
#endif
#ifdef IMU_SENSOR
#include "IMUModule.h"
#endif
#ifdef STORAGE
#include "StorageModule.h"
#endif
#if defined(NETWORKING) && defined(OTA_UPDATE)
#include "OTAUpdateModule.h"
#endif

#ifdef UI_DISPLAY
#include "face/FaceManager.h"
#endif
#if defined(UI_DISPLAY) && defined(NETWORKING) && defined(NETWORK_TIME)
#include "face/ClockFace.h"
#endif
#if defined(UI_DISPLAY) && defined(ENVIRONMENT_SENSOR)
#include "face/TemperatureFace.h"
#include "face/HumidityFace.h"
#include "face/IAQFace.h"
#include "face/PressureFace.h"
#endif
#if defined(UI_DISPLAY) && defined(LIGHT_LEVEL_SENSOR)
#include "face/LightLevelFace.h"
#endif
#if defined(UI_DISPLAY) && defined(IMU_SENSOR)
#include "face/IMUFace.h"
#include "face/DGFace.h"
#include "face/AutoFace.h"
#endif
#if defined(UI_DISPLAY) && (defined(LIGHT_LEVEL_SENSOR) || defined(IMU_SENSOR) || defined(ENVIRONMENT_SENSOR))
#include "face/MeasuresFace.h"
#endif

void loadModules()
{
    ModuleRegistry& registry = ModuleRegistry::instance();

    // Load order is significant - specify dependent modules first
    static SystemModule systemModule("System Handler");
    registry.registerModule(systemModule);

#ifdef NETWORKING
    static NetworkModule networkModule("Network Overlord");
#if defined(NETWORK_TIME)
    static TimeModule timeModule("Time Syncer");
#endif
#if defined(NETWORKING) && defined(OTA_UPDATE)
    static OTAUpdateModule otaUpdateModule("OTA Updates");
#endif
#endif

#ifdef UI_DISPLAY
    static DisplayModule displayModule("Round Display");
    static FaceManager faceManager("Face Manager", 1000);   // Must be loaded prior to loading faces
#if defined(NETWORK_TIME)
    static ClockFace clockFace("Clock", 500);
#endif
#ifdef ENVIRONMENT_SENSOR
    static TemperatureFace temperatureFace("Inside Temperature");
    static HumidityFace humidityFace("Humidity");
    static IAQFace iaqFace("IAQ");
    static PressureFace pressureFace("Pressure");
#endif
#ifdef LIGHT_LEVEL_SENSOR
    static LightLevelFace lightLevelFace("Light Level");
#endif
#if defined(LIGHT_LEVEL_SENSOR) || defined(IMU_SENSOR) || defined(ENVIRONMENT_SENSOR)
    static MeasuresFace measuresFace("Measures");
#endif
#ifdef IMU_SENSOR
    static DGFace dgFace("DG", 50);
    static AutoFace autoFace("Auto", 0);
    static IMUFace imuFace("Y/P/R");
#endif
#endif

#if defined(SHIFTER_KNOB)
    static ShifterModule shifterModule("Shifter LED", SHIFTER_PIN);
#ifdef HAPTIC
    static HapticModule hapticForwardModule("Haptic Feedback Forward", HAPTIC_PIN_1);
    // static HapticModule hapticReverseModule("Haptic Feedback Reverse", HAPTIC_PIN_2);
#endif
#endif

#ifdef CANBUS
    static CANBusModule canBusModule("CAN Bus Transceiver", CAN_TX_PIN, CAN_RX_PIN, CANBUS_SPEED);
#endif
#ifdef BUTTON
    static ButtonModule buttonModule("Rotary Button", ROTARY_BUTTON_PIN);
#endif
#ifdef BUZZER_SOUND
    static BuzzerModule buzzerModule("Main Buzzer", BUZZER_PIN);
#endif
#ifdef ENCODER
    static EncoderModule encoderModule("Rotary Encoder", ENCODER_A, ENCODER_B, ENCODER_DEFAULT_STEPS);
#endif
#ifdef LIGHT_LEVEL_SENSOR
    static LightLevelModule lightLevelModule("Light Level Sensor", 2000);
#endif
#ifdef ENVIRONMENT_SENSOR
    static EnvironmentModule environmentModule("Environment Sensor", (unsigned int)round(1/SAMPLE_RATE));
#endif
#ifdef IMU_SENSOR
    static IMUModule imuModule("Inertial Measurement Unit", 50);
#endif
#ifdef STORAGE
    static StorageModule storageModule("SD Card Storage");
#endif

#ifdef UI_DISPLAY
    registry.registerModule(displayModule);
#endif
#ifdef NETWORKING
    registry.registerModule(networkModule);
#endif
#if defined(NETWORKING) && defined(NETWORK_TIME)
    registry.registerModule(timeModule);
#endif
#ifdef CANBUS
    registry.registerModule(canBusModule);
#endif
#ifdef BUTTON
    registry.registerModule(buttonModule);
#endif
#ifdef BUZZER_SOUND
    registry.registerModule(buzzerModule);
#endif
#ifdef ENCODER
    registry.registerModule(encoderModule);
#endif
#ifdef LIGHT_LEVEL_SENSOR
    registry.registerModule(lightLevelModule);
#endif
#ifdef HAPTIC
    registry.registerModule(hapticForwardModule);
    // registry.registerModule(hapticReverseModule);
#endif
#ifdef SHIFTER_KNOB
    registry.registerModule(shifterModule);
#endif
#ifdef ENVIRONMENT_SENSOR
    registry.registerModule(environmentModule);
#endif
#ifdef IMU_SENSOR
    registry.registerModule(imuModule);
#endif
#ifdef STORAGE
    registry.registerModule(storageModule);
#endif
#if defined(NETWORKING) && defined(OTA_UPDATE)
    registry.registerModule(otaUpdateModule);
#endif

#ifdef UI_DISPLAY
    registry.registerModule(faceManager);
#if defined(NETWORKING) && defined(NETWORK_TIME)
    registry.registerModule(clockFace);
#endif
#if defined(IMU_SENSOR)
    registry.registerModule(dgFace);
    registry.registerModule(autoFace);
    registry.registerModule(imuFace);
#endif
#if defined(ENVIRONMENT_SENSOR)
    registry.registerModule(temperatureFace);
    registry.registerModule(humidityFace);
    registry.registerModule(iaqFace);
    registry.registerModule(pressureFace);
#endif
#if defined(LIGHT_LEVEL_SENSOR)
    registry.registerModule(lightLevelFace);
#endif
#if defined(LIGHT_LEVEL_SENSOR) || defined(IMU_SENSOR) || defined(ENVIRONMENT_SENSOR))
    registry.registerModule(measuresFace);
#endif
#endif
}
