#pragma once
#include <Arduino.h>

#define SOUND_SPEED_CM_US 0.034f // cm per microsecond
#define MAX_DISTANCE_CM 1000     // from datasheet 8 m but use 10 m for leeway
#define TRIGGER_PULSE_US 10      // must be > 1 ms
#define ECHO_TIMEOUT_US (uint32_t)(MAX_DISTANCE_CM * 2 / SOUND_SPEED_CM_US)

class Sonar {
public:
  Sonar(uint8_t trigPin, uint8_t echoPin);

  void begin();
  long read();
  float readCm();

private:
  uint8_t _trig;
  uint8_t _echo;
};
