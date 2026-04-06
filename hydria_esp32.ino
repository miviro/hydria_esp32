#include "src/config.h"
#include "src/debug.h"
#include "src/sonar.h"
#include "src/turbidity.h"
#include "src/humidity.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <sys/time.h>

// TODO: turn on sensors only when awaken and measuring
RTC_DATA_ATTR static time_t lastMeasureTime = 0;
RTC_DATA_ATTR static int    extWakeCount    = 0;

static Sonar sonar(TRIGGER_PIN, ECHO_PIN);
static Turbidity turbidity(TURBIDITY_PIN);
static Humidity humidity(HUMIDITY_PIN);

static void takeReadings() {
    float sonarSum = 0, turbiditySum = 0, humiditySum = 0;

    for (int i = 0; i < READINGS_PER_MEASUREMENT; i++) {
        sonar.trigger();
        turbiditySum += turbidity.read();
        humiditySum  += humidity.read();
        sonarSum     += sonar.listenCm();
        if (i < READINGS_PER_MEASUREMENT - 1) delay(60);
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

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_t wakeTime = tv.tv_sec;

    bool timerWakeup = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
    bool extWakeup   = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0;

    if (extWakeup) extWakeCount++;

    uint32_t sinceMeasureS;
    if (lastMeasureTime == 0) {
      sinceMeasureS = 0;
    } else {
      sinceMeasureS = (uint32_t)(wakeTime - lastMeasureTime);
    }
    LOG("Woke up (%s), ext count %d/%d, %u s since last measure\n",
        timerWakeup ? "timer" : "ext", extWakeCount, EXT_WAKEUP_THRESHOLD, sinceMeasureS);

    bool shouldMeasure = timerWakeup || (extWakeCount >= EXT_WAKEUP_THRESHOLD);
    if (shouldMeasure) {
        sonar.begin();
        turbidity.begin();
        humidity.begin();
        takeReadings();
        lastMeasureTime = wakeTime;
        extWakeCount    = 0;
    }

    goToSleep();
}

void loop() {}
