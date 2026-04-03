#pragma once
#include <Arduino.h>

class Turbidity {
public:
  Turbidity(uint8_t pin);

  void begin();
  int read(); // raw ADC value (0–4095); lower = more turbid
};
