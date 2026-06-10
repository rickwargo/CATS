#include "mathHelpers.h"

float percentChange(float oldVal, float newVal) {
    constexpr float FACTOR = 1.0;   // 1.0 is 100%

    if (oldVal == 0) {
        return (newVal == 0) ? 0 : FACTOR; // Handle division by zero
    }
    return ((newVal - oldVal) / oldVal) * FACTOR;
}
