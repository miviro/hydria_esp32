#include "humidity.h"
#include "debug.h"

static uint8_t _pin;

Humidity::Humidity(uint8_t pin) { _pin = pin; }

void Humidity::begin() { pinMode(_pin, INPUT); }

int Humidity::read() {
  // Typical Chinese capacitive sensor: higher value = wetter, lower = drier
  // (resistive sensors are inverted — swap if needed)
  return analogRead(_pin);
}
