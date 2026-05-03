#include "src/config.h"
#include "src/debug.h"
#include "src/display.h"
#include "src/battery.h"
#include "src/sonar.h"
#include "src/turbidity.h"
#include "src/humidity.h"
#include "src/lora.h"
#include <esp_sleep.h>
#include <driver/gpio.h>
#include <driver/rtc_io.h>

RTC_DATA_ATTR static int     extWakeCount = 0;
RTC_DATA_ATTR static uint8_t rollingCode  = 0; // 4-bit counter incremented every send

static Sonar sonar(TRIGGER_PIN, ECHO_PIN);
static Turbidity turbidity(TURBIDITY_PIN);
static Humidity humidity(HUMIDITY_PIN);

static Readings takeReadings() {
    float sonarSum = 0, turbiditySum = 0, humiditySum = 0;
    int sonarCount = 0;

    for (int i = 0; i < READINGS_PER_MEASUREMENT; i++) {
        sonar.trigger();
        turbiditySum += turbidity.read();
        humiditySum  += humidity.read();
        float s = sonar.listenCm();
        if (s >= 0) { sonarSum += s; sonarCount++; }
        if (i < READINGS_PER_MEASUREMENT - 1) delay(60);
    }

    Readings r;
    r.sonarCm   = (sonarCount > 0) ? (sonarSum / sonarCount) : -1.0f;
    r.turbidity = turbiditySum / READINGS_PER_MEASUREMENT;
    r.humidity  = humiditySum  / READINGS_PER_MEASUREMENT;
    r.battery   = batteryPercent();

    LOG("avg sonar:     %.1f cm\n", r.sonarCm);
    LOG("avg turbidity: %.0f\n",    r.turbidity);
    LOG("avg humidity:  %.0f\n",    r.humidity);
    LOG("battery:       %d %%\n",   r.battery);

    return r;
}

static HydriaFrame buildFrame(const Readings &r, uint16_t wakeCount, uint8_t rolling) {
    HydriaFrame f;
    f.header    = FRAME_HEADER(DEVICE_ID, rolling);
    f.wakeCount = wakeCount;
    f.sonarMm   = (r.sonarCm < 0) ? 0xFFFFu : (uint16_t)(r.sonarCm * 10.0f + 0.5f);
    f.turbidity = (uint16_t)(r.turbidity + 0.5f);
    f.humidity  = (uint16_t)(r.humidity  + 0.5f);
    f.battery   = (uint8_t)r.battery;
    return f;
}

static void goToSleep() {
    LOG("Sleeping for %d s...\n", SLEEP_INTERVAL_S);
#if DEBUG
    Serial.flush();
#endif
    digitalWrite(VEXT_CTRL, HIGH);          // Vext off: kills OLED + sensor rail
    // GPIO 36 isn't an RTC pin; latch its level so it survives deep sleep.
    gpio_hold_en((gpio_num_t)VEXT_CTRL);
    gpio_deep_sleep_hold_en();
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_INTERVAL_S * 1000000ULL);
    // Wake on GPIO -> GND. Pullup uses VDD3P3_RTC, which is always alive in
    // deep sleep, so wake works on battery (the header 3V3 pin tracks Vext
    // and is dead while VEXT_CTRL is HIGH).
    rtc_gpio_init(WAKEUP_PIN);
    rtc_gpio_set_direction(WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(WAKEUP_PIN);
    rtc_gpio_pullup_en(WAKEUP_PIN);
    esp_sleep_enable_ext1_wakeup(1ULL << WAKEUP_PIN, ESP_EXT1_WAKEUP_ANY_LOW);
    esp_deep_sleep_start();
}

void setup() {
    gpio_hold_dis((gpio_num_t)VEXT_CTRL);
    gpio_deep_sleep_hold_dis();
    pinMode(VEXT_CTRL, OUTPUT);
    digitalWrite(VEXT_CTRL, HIGH); // Vext off until we actually measure

#if DEBUG
    Serial.begin(115200);
    delay(100); // give USB CDC time to initialise after each wake cycle
#endif

    bool timerWakeup = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
    bool extWakeup   = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1;

    if (extWakeup) extWakeCount++;

    LOG("Woke up (%s), ext count %d/%d\n",
        timerWakeup ? "timer" : "ext", extWakeCount, EXT_WAKEUP_THRESHOLD);

    bool shouldMeasure = timerWakeup || (extWakeCount >= EXT_WAKEUP_THRESHOLD);
    if (shouldMeasure) {
        digitalWrite(VEXT_CTRL, LOW); // Vext on: powers OLED + sensors
        delay(500);                   // let the rail + sensors stabilise
        sonar.begin();
        turbidity.begin();
        humidity.begin();

#if DEBUG
        displayBegin();
        displayStatus("reading");
#endif
        Readings r = takeReadings();

        uint16_t wakeCount = (uint16_t)extWakeCount;
        extWakeCount = 0;

        HydriaFrame frame = buildFrame(r, wakeCount, rollingCode);
        rollingCode = (rollingCode + 1) & 0x0F;

#if DEBUG
        loraPrintFrame(frame);
        displayReadings(r.sonarCm, (int)r.turbidity, (int)r.humidity, r.battery, wakeCount);
        delay(5000);
        displayStatus("sending");
#endif
        loraBegin();
        loraSend(frame);
#if DEBUG
        displayStatus("ok");
        delay(500);
        displayOff();
#endif
    }

    goToSleep();
}

void loop() {}
