#pragma once
#include <Arduino.h>

// 1 = debugging, 0 = deployment
#define DEBUG 1

#if DEBUG
  #define LOG(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
  #define LOG(fmt, ...)
#endif
