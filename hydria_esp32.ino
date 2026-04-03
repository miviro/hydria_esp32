#include "src/config.h"
#include "src/sonar.h"
#include "src/turbidity.h"
#include "src/humidity.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>

// Module instances
static Sonar sonar(TRIGGER_PIN, ECHO_PIN);
static Turbidity turbidity(TURBIDITY_PIN);
static Humidity humidity(HUMIDITY_PIN);

// Helpers
static void takeReadings() {
    for (int i = 0; i < 3; i++) {
        sonar.readCm();
        turbidity.read();
        humidity.read();
        if (i < 2) delay(333); // ~3 readings per second
    }
}

static void goToSleep() {
    Serial.printf("Sleeping for %d s...\n", SLEEP_INTERVAL_S);
    Serial.flush();
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_INTERVAL_S * 1000000ULL);
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1);
    rtc_gpio_pulldown_en(WAKEUP_PIN);
    rtc_gpio_pullup_dis(WAKEUP_PIN);
    esp_deep_sleep_start();
}

// Arduino entry points
void setup() {
    Serial.begin(115200);
    Serial.println("Boot");
    sonar.begin();
    turbidity.begin();
    humidity.begin();
    takeReadings();
    goToSleep();
}

void loop() {}
