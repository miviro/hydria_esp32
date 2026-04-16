#include "src/config.h"
#include "src/debug.h"
#include "src/display.h"
#include "src/sonar.h"
#include "src/turbidity.h"
#include "src/humidity.h"
#include "src/lora.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include <sys/time.h>

RTC_DATA_ATTR static time_t lastMeasureTime = 0;
RTC_DATA_ATTR static int    extWakeCount    = 0;

static Sonar sonar(TRIGGER_PIN, ECHO_PIN);
static Turbidity turbidity(TURBIDITY_PIN);
static Humidity humidity(HUMIDITY_PIN);

static Readings takeReadings() {
    float sonarSum = 0, turbiditySum = 0, humiditySum = 0;

    for (int i = 0; i < READINGS_PER_MEASUREMENT; i++) {
        sonar.trigger();
        turbiditySum += turbidity.read();
        humiditySum  += humidity.read();
        sonarSum     += sonar.listenCm();
        if (i < READINGS_PER_MEASUREMENT - 1) delay(60);
    }

    Readings r;
    r.sonarCm   = sonarSum     / READINGS_PER_MEASUREMENT;
    r.turbidity = turbiditySum / READINGS_PER_MEASUREMENT;
    r.humidity  = humiditySum  / READINGS_PER_MEASUREMENT;

    LOG("avg sonar:     %.1f cm\n", r.sonarCm);
    LOG("avg turbidity: %.0f\n",    r.turbidity);
    LOG("avg humidity:  %.0f\n",    r.humidity);

    return r;
}

static HydriaFrame buildFrame(const Readings &r, bool extWakeup, uint32_t sinceMeasureS) {
    HydriaFrame f;
    f.flags         = extWakeup ? FRAME_FLAG_EXT_WAKEUP : 0;
    f.sinceMeasureS = (lastMeasureTime == 0)
                          ? FRAME_NO_PRIOR_MEASURE
                          : (uint16_t)(sinceMeasureS > 0xFFFEu ? 0xFFFEu : sinceMeasureS);
    f.sonarMm       = (uint16_t)(r.sonarCm * 10.0f + 0.5f);
    f.turbidity     = (uint16_t)(r.turbidity + 0.5f);
    f.humidity      = (uint16_t)(r.humidity  + 0.5f);
    return f;
}

static void goToSleep() {
    LOG("Sleeping for %d s...\n", SLEEP_INTERVAL_S);
#if DEBUG
    Serial.flush();
#endif
    displayOff();
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_INTERVAL_S * 1000000ULL);
    rtc_gpio_init(WAKEUP_PIN);
    rtc_gpio_set_direction(WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pullup_dis(WAKEUP_PIN);
    rtc_gpio_pulldown_en(WAKEUP_PIN);
    esp_sleep_enable_ext1_wakeup(1ULL << WAKEUP_PIN, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}

void setup() {
#if DEBUG
    Serial.begin(115200);
    delay(100); // give USB CDC time to initialise after each wake cycle
#endif
    displayBegin();
    displayImage();

    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(SENSOR_POWER_PIN, HIGH);

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    time_t wakeTime = tv.tv_sec;

    bool timerWakeup = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
    bool extWakeup   = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1;

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
        digitalWrite(SENSOR_POWER_PIN, HIGH);
        delay(500);  // let sensors stabilize
        sonar.begin();
        turbidity.begin();
        humidity.begin();

        Readings r = takeReadings();

        digitalWrite(SENSOR_POWER_PIN, LOW);
        lastMeasureTime = wakeTime;
        extWakeCount    = 0;

        HydriaFrame frame = buildFrame(r, extWakeup, sinceMeasureS);

#if DEBUG
        loraPrintFrame(frame);
#endif

        loraBegin();
        loraSend(frame);
    }

    goToSleep();
}

void loop() {}
