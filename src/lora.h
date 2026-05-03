#pragma once
#include <Arduino.h>
#include "config.h"

// Header byte: low nibble = DEVICE_ID, high nibble = rolling counter (anti-replay)
#define FRAME_DEVICE_ID(hdr)   ((hdr) & 0x0F)
#define FRAME_ROLLING(hdr)     (((hdr) >> 4) & 0x0F)
#define FRAME_HEADER(id, roll) ((uint8_t)(((id) & 0x0F) | (((roll) & 0x0F) << 4)))

// 16-byte packed telemetry frame (10 data + 6-byte truncated HMAC-SHA256)
// Wake source is implicit in wakeCount: == EXT_WAKEUP_THRESHOLD means ext-triggered send,
// anything else means the periodic timer fired.
#pragma pack(push, 1)
struct HydriaFrame {
    uint8_t  header;          // low nibble: device id (0–15); high nibble: rolling counter (0–15)
    uint16_t wakeCount;       // number of ext wakeups accumulated since last send
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
