#include "src/config.h"
#include "src/sonar.h"

// Module instances
static Sonar sonar(TRIGGER_PIN, ECHO_PIN);

// Helpers
static void takeReadings() {
    float distance = sonar.readCm();
    // Add more sensors here as the project grows.
}

static void goToSleep() {
    Serial.printf("Sleeping for %d s...\n", SLEEP_INTERVAL_S);
    Serial.flush();

}

// Arduino entry points
void setup() {
    Serial.begin(115200);
    Serial.println("Boot");
    sonar.begin();

}

void loop() {
    takeReadings();
    delay(1000);
}
