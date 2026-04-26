#pragma once
#include <Arduino.h>
#include "config.h"

// Frame flag bits
#define FRAME_FLAG_EXT_WAKEUP 0x01  // bit 0: 0 = timer wakeup, 1 = ext wakeup

// sinceMeasureS sentinel: no prior measurement (first boot)
#define FRAME_NO_PRIOR_MEASURE 0xFFFFu

// 16-byte packed telemetry frame (10 data + 6-byte truncated HMAC-SHA256)
#pragma pack(push, 1)
struct HydriaFrame {
    uint8_t  flags;           // see FRAME_FLAG_* above
    uint16_t sinceMeasureS;   // seconds since last measurement; FRAME_NO_PRIOR_MEASURE on first boot
    uint16_t sonarMm;         // sonar distance in mm (0.1 cm resolution, 0–65535 mm)
    uint16_t turbidity;       // raw ADC 0–4095
    uint16_t humidity;        // raw ADC 0–4095
    uint8_t  battery;         // 0–100 %
    uint8_t  hmac[6];         // first 6 bytes of HMAC-SHA256(key, data fields above)
};
#pragma pack(pop)

// Sensor readings (averaged)
struct Readings {
    float sonarCm;
    float turbidity;
    float humidity;
    int   battery;  // 0–100 %
};

bool loraBegin();
bool loraSend(HydriaFrame &frame); // fills frame.hmac then transmits

#if DEBUG
void loraPrintFrame(const HydriaFrame &frame);
#endif
