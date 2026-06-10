#include <cstdarg> // Required for va_list, va_start, va_end, etc.
#include <Arduino.h>
#include "say.h"


void say(const char* format, ...) {
    char text[160];
    memset(text, 0, 160);
    va_list args;
    va_start(args, format);  // Initialize args with the variable arguments
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);

    Serial.println(text);
    Serial.flush();
}
