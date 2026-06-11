#pragma once

#include <Measure.h>

struct UnitConverter
{
    const char* from;
    const char* to;
    float scale;
    float offset;
};

constexpr UnitConverter FtoC
{
    "°F",
    "°C",
    5.0f / 9.0f,
    -32.0f
};

constexpr UnitConverter mbarToInHg
{
    "mbar",
    "inHg",
    0.02952998057228f,
    0.0f
};

template<typename T>
void convert(
    Measure<T>& measure,
    const UnitConverter& conv)
{
    measure.set(static_cast<T>((measure.get() + conv.offset) * conv.scale));
    measure.unit(conv.to);
}
