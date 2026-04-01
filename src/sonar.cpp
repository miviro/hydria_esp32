#include "sonar.h"
#include "esp32-hal.h"

Sonar::Sonar(uint8_t trigPin, uint8_t echoPin)
    : _trig(trigPin), _echo(echoPin) {}

void Sonar::begin() {
  pinMode(_trig, OUTPUT);
  pinMode(_echo, INPUT);
  digitalWrite(_trig, LOW);
}

long Sonar::read() {
  // Ensure trigger line is low before pulse
  digitalWrite(_trig, LOW);
  delayMicroseconds(5);

  // Send >1 ms TTL pulse (Mode 2 low-power trigger)
  digitalWrite(_trig, HIGH);
  delayMicroseconds(TRIGGER_PULSE_US);
  digitalWrite(_trig, LOW);

  delayMicroseconds(1000); // let trigger transient settle before listening
  long duration = pulseIn(_echo, HIGH, 100000UL); // 100 ms → ~17 m leeway
  return duration;
}

float Sonar::readCm() {
  long duration = read();

  if (duration == 0) {
    Serial.println("sonar: no echo");
    return -1.0f;
  }

  float cm = (duration * SOUND_SPEED_CM_US) / 2.0f;
  Serial.printf("sonar: %.1f cm\n", cm);
  return cm;
}
