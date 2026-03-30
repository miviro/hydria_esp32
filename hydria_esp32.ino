#include "src/config.h"
#include "src/sonar.h"

// Module instances
static Sonar sonar(TRIGGER_PIN, ECHO_PIN);

// Helpers
static void takeReadings() {
    float distance = sonar.readCm();

    if (distance < 0) {
        Serial.println("Sonar: no echo / out of range");
    } else {
        Serial.printf("Sonar: %.1f cm\n", distance);
    }

    // Add more sensors here as the project grows.
}

static void goToSleep() {
    Serial.printf("Ssleeping for %d s...\n", SLEEP_INTERVAL_S);
    Serial.flush();

    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_INTERVAL_S * 1000000ULL);
    esp_deep_sleep_start();
    // Execution never reaches here — deep sleep resets the chip.
}

// Arduino entry points
void setup() {
    Serial.begin(115200);
    Serial.println("Boot");
    sonar.begin();

    takeReadings();
    goToSleep();
}

void loop() {
    // Intentionally empty.
    // Deep sleep resets the CPU, so execution always re-enters setup().
}
