#include "sonar.h"

Sonar::Sonar(uint8_t trigPin, uint8_t echoPin)
    : _trig(trigPin), _echo(echoPin) {}

void Sonar::begin() {
    pinMode(_trig, OUTPUT);
    pinMode(_echo, INPUT);
    digitalWrite(_trig, LOW);
}

float Sonar::readCm() {
    // Ensure trigger line is low before pulse
    digitalWrite(_trig, LOW);
    delayMicroseconds(5);

    // Send >1 ms TTL pulse (Mode 2 low-power trigger)
    digitalWrite(_trig, HIGH);
    delay(TRIGGER_PULSE_MS);
    digitalWrite(_trig, LOW);

    long duration = pulseIn(_echo, HIGH, ECHO_TIMEOUT_US);

    if (duration == 0) return -1.0f;

    return (duration * SOUND_SPEED_CM_US) / 2.0f;
}
