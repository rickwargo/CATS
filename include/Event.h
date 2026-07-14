#pragma once

class Event {
    public:
    enum Type {
        EventDiscarded,
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
        ToggleMusic,
        PreviousMusic,
        NextMusic,
        SystemReset,
        OTAUpdateRequest,
        LastType
    } type;

    void* source = nullptr;
    // time_t timestamp = 0;
    int data = 0;

    const char *name[(int)LastType] = {
        "Event Discarded",
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
        "Toggle Music",
        "Previous Music",
        "Next Music",
        "System Reset"
    };

    [[nodiscard]] const char *getName() const
    {
        return name[type];
    }
};
