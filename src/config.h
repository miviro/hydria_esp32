#pragma once

// Deep Sleep
#define SLEEP_INTERVAL_S     10
#define MEASURE_INTERVAL_S   300  // take readings if this many seconds have passed

// Wakeup
#define WAKEUP_PIN           GPIO_NUM_12
#define WAKEUP_COUNT_THRESHOLD 5  // take readings every N wakeups

// Readings
#define READINGS_PER_MEASUREMENT 5  // average over this many samples

// Sonar Pins
#define TRIGGER_PIN 26
#define ECHO_PIN    27

// Turbidity Pin
#define TURBIDITY_PIN 33

// Humidity Pin
#define HUMIDITY_PIN 32
