#pragma once

// Deep Sleep
#define SLEEP_INTERVAL_S       300  // 5 minutes

// Wakeup
#define WAKEUP_PIN             GPIO_NUM_12
#define EXT_WAKEUP_THRESHOLD   5    // measure after this many external wakeups

// Readings
#define READINGS_PER_MEASUREMENT 5  // average over this many samples

// Sonar Pins
#define TRIGGER_PIN 26
#define ECHO_PIN    27

// Turbidity Pin
#define TURBIDITY_PIN 33

// Humidity Pin
#define HUMIDITY_PIN 32

// Sensor Power Switch (PNP transistor base, active LOW)
#define SENSOR_POWER_PIN 25
