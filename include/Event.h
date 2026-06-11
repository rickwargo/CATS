#pragma once

class Event {
    public:
    enum Type {
        ButtonClick,
        ButtonLong,
        ButtonDouble,
        ButtonTriple,
        EncoderCCW,
        EncoderCW,
        EncoderValueChanged,
        LightLevelChanged,
        LightLevelDark,
        LightLevelLight,
        HapticFeedbackRequest,
        IAQChanged,
        IAQModerate,
        IAQVeryUnhealthy,
        TemperatureChanged,
        PressureChanged,
        HumidityChanged,
        YawChanged,
        PitchChanged,
        RollChanged,
        SystemReset,
        LastType
    } type;

    void* source = nullptr;
    int data = 0;

    const char *name[(int)LastType] = {
        "Button Click",
        "Button Long",
        "Button Double",
        "Button Triple",
        "Encoder CCW",
        "Encoder CW",
        "Encoder Value",
        "Light Level",
        "Light Level Dark",
        "Light Level Light",
        "Haptic Feedback Request",
        "IAQ",
        "IAQ Moderate",
        "IAQ Very Unhealthy",
        "Temperature",
        "Pressure",
        "Humidity",
        "Yaw",
        "Pitch",
        "Roll",
        "System Reset"
    };

    [[nodiscard]] const char *getName() const
    {
        return name[type];
    }
};
