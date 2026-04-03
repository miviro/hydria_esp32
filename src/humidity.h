#pragma once
#include <Arduino.h>

class Humidity {
public:
  Humidity(uint8_t pin);

  void begin();
  int read(); // raw ADC value (0–4095); lower = drier, higher = wetter
};
