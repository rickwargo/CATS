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
        "ButtonClick",
        "ButtonLong",
        "ButtonDouble",
        "ButtonTriple",
        "EncoderCCW",
        "EncoderCW",
        "EncoderValueChanged",
        "LightLevelChanged",
        "LightLevelDark",
        "LightLevelLight",
        "HapticFeedbackRequest",
        "IAQChanged",
        "IAQModerate",
        "IAQVeryUnhealthy",
        "TemperatureChanged",
        "PressureChanged",
        "HumidityChanged",
        "YawChanged",
        "PitchChanged",
        "RollChanged",
        "SystemReset"
    };

    [[nodiscard]] const char *getName() const
    {
        return name[type];
    }
};
