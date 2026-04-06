#include "src/config.h"
#include "src/debug.h"
#include "src/sonar.h"
#include "src/turbidity.h"
#include "src/humidity.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>

RTC_DATA_ATTR static int wakeCount = 0;
RTC_DATA_ATTR static uint32_t secondsSinceLastMeasure = 0;

static Sonar sonar(TRIGGER_PIN, ECHO_PIN);
static Turbidity turbidity(TURBIDITY_PIN);
static Humidity humidity(HUMIDITY_PIN);

static void takeReadings() {
    float sonarSum = 0, turbiditySum = 0, humiditySum = 0;

    for (int i = 0; i < READINGS_PER_MEASUREMENT; i++) {
        sonarSum     += sonar.readCm();
        turbiditySum += turbidity.read();
        humiditySum  += humidity.read();
        if (i < READINGS_PER_MEASUREMENT - 1) delay(200);
    }

    LOG("avg sonar:     %.1f cm\n", sonarSum     / READINGS_PER_MEASUREMENT);
    LOG("avg turbidity: %.0f\n",    turbiditySum / READINGS_PER_MEASUREMENT);
    LOG("avg humidity:  %.0f\n",    humiditySum  / READINGS_PER_MEASUREMENT);
}

static void goToSleep() {
    LOG("Sleeping for %d s...\n", SLEEP_INTERVAL_S);
#if DEBUG
    Serial.flush();
#endif
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_INTERVAL_S * 1000000ULL);
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1);
    rtc_gpio_pulldown_en(WAKEUP_PIN);
    rtc_gpio_pullup_dis(WAKEUP_PIN);
    esp_deep_sleep_start();
}

void setup() {
#if DEBUG
    Serial.begin(115200);
#endif

    wakeCount++;
    secondsSinceLastMeasure += SLEEP_INTERVAL_S;
    LOG("Wake #%d, %d s since last measure\n", wakeCount, secondsSinceLastMeasure);

    bool countReached = wakeCount >= WAKEUP_COUNT_THRESHOLD;
    bool timeReached  = secondsSinceLastMeasure >= MEASURE_INTERVAL_S;

    if (countReached || timeReached) {
        sonar.begin();
        turbidity.begin();
        humidity.begin();
        takeReadings();
        wakeCount = 0;
        secondsSinceLastMeasure = 0;
    }

    goToSleep();
}

void loop() {}
