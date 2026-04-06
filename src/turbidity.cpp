#include "turbidity.h"
#include "debug.h"

static uint8_t _pin;

Turbidity::Turbidity(uint8_t pin) { _pin = pin; }

void Turbidity::begin() { pinMode(_pin, INPUT); }

int Turbidity::read() {
  // DFRobot analog: higher value = clearer, lower value = more turbid
  return analogRead(_pin);
}
