#pragma once

void say(const char* format, ...);
// #define say(x, ...) Serial.printf(F(x) __VA_OPT__(,) __VA_ARGS__)
