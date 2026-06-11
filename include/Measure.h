#pragma once

#include <cmath>
#include <type_traits>
#include <cstdio>

class IMeasure
{
public:
    virtual ~IMeasure() = default;
    [[nodiscard]] virtual const char* unit() const = 0;
    [[nodiscard]] virtual uint8_t precision() const = 0;
    virtual void format(char* buffer, size_t len) const = 0;
};

template<typename T>
class Measure : public IMeasure
{
    static_assert(std::is_arithmetic_v<T>, "Measure requires numeric type");

public:
    constexpr Measure() = default;

    constexpr Measure(
        T value,
        const char* unit = "",
        uint8_t precision = 0)
        : _value(value),
          _unit(unit),
          _precision(precision)
    {
    }

    [[nodiscard]] const char* unit() const override
    {
        return _unit;
    }

    [[nodiscard]] uint8_t precision() const override
    {
        return _precision;
    }

    void format(char* buffer, size_t len) const override
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            snprintf(
                buffer,
                len,
                "%.*f",
                _precision,
                static_cast<double>(_value)
            );
        }
        else
        {
            snprintf(
                buffer,
                len,
                "%d",
                static_cast<int>(_value)
            );
        }
    }

    //---------------------------------------------------------------------
    // Value access
    //---------------------------------------------------------------------

    constexpr T value() const
    {
        return _value;
    }

    constexpr void value(T newValue)
    {
        _changed = displayChanged(_value, newValue);
        _value = newValue;
    }

    //---------------------------------------------------------------------
    // Property aliases
    //---------------------------------------------------------------------

    constexpr T get() const
    {
        return value();
    }

    constexpr void set(T v)
    {
        value(v);
    }

    //---------------------------------------------------------------------
    // Display metadata
    //---------------------------------------------------------------------

    constexpr void precision(uint8_t digits)
    {
        _precision = digits;
    }

    constexpr void unit(const char* u)
    {
        _unit = u;
    }

    //---------------------------------------------------------------------
    // Change detection
    //---------------------------------------------------------------------

    [[nodiscard]] constexpr bool changed() const
    {
        return _changed;
    }

    constexpr void clearChanged()
    {
        _changed = false;
    }

private:
    bool displayChanged(T oldVal, T newVal) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            const double scale =
                std::pow(10.0, static_cast<double>(_precision));

            const auto oldRounded =
                std::llround(oldVal * scale);

            const auto newRounded =
                std::llround(newVal * scale);

            return oldRounded != newRounded;
        }
        else
        {
            return oldVal != newVal;
        }
    }

private:
    T           _value {};
    const char* _unit {""};
    uint8_t     _precision {0};
    bool        _changed {false};
};
