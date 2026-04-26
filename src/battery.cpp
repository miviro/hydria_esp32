#include "battery.h"
#include "config.h"
#include "debug.h"
#include <Arduino.h>

int batteryPercent() {
    pinMode(VBAT_ADC_CTRL, OUTPUT);
    digitalWrite(VBAT_ADC_CTRL, HIGH); // HIGH enables the voltage divider on Heltec V4

    delay(10);
    analogSetAttenuation(ADC_11db); // full 0–3.3 V range
    int raw = analogRead(VBAT_ADC_PIN);

    digitalWrite(VBAT_ADC_CTRL, LOW); // disable to save power

    // Vbat = raw * (3.3 / 4095) * 4.9  (390K/100K divider, 12-bit ADC, 3.3 V ref)
    float voltage = (raw / 4095.0f) * 3.3f * 4.9f;
    LOG("battery raw: %d, voltage: %.2f V\n", raw, voltage);

    // 3.2 V = empty (0 %), 4.2 V = full (100 %)
    int pct = (int)((voltage - 3.2f) / (4.2f - 3.2f) * 100.0f);
    if (pct < 0)   pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}
