#include "sonar.h"
#include "debug.h"
#include "esp32-hal.h"

Sonar::Sonar(uint8_t trigPin, uint8_t echoPin)
    : _trig(trigPin), _echo(echoPin) {}

void Sonar::begin() {
  pinMode(_trig, OUTPUT);
  pinMode(_echo, INPUT);
  digitalWrite(_trig, LOW);
}

void Sonar::trigger() {
  digitalWrite(_trig, LOW);
  delayMicroseconds(5);
  digitalWrite(_trig, HIGH);
  delayMicroseconds(TRIGGER_PULSE_US);
  digitalWrite(_trig, LOW);
  // caller can do other work here while echo is in flight
}

float Sonar::listenCm() {
  delayMicroseconds(1000); // let trigger transient settle
  long duration = pulseIn(_echo, HIGH, 100000UL);
  if (duration == 0)
    return -1.0f;
  return (duration * SOUND_SPEED_CM_US) / 2.0f;
}

long Sonar::read() {
  trigger();
  delayMicroseconds(1000);
  return pulseIn(_echo, HIGH, 100000UL);
}

float Sonar::readCm() {
  trigger();
  return listenCm();
}
