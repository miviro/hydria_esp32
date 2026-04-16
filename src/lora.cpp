#include "lora.h"
#include "debug.h"
#include <RadioLib.h>
#include <SPI.h>

static SX1262 radio = new Module(LORA_SS, LORA_DIO1, LORA_RST, LORA_BUSY);

bool loraBegin() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    int state = radio.begin(LORA_FREQ);
    if (state != RADIOLIB_ERR_NONE) {
        LOG("LoRa init failed: %d\n", state);
        return false;
    }
    radio.setOutputPower(LORA_POWER);
    LOG("LoRa ready\n");
    return true;
}

bool loraSend(const HydriaFrame &frame) {
    int state = radio.transmit(reinterpret_cast<const uint8_t *>(&frame), sizeof(frame));
    if (state != RADIOLIB_ERR_NONE) {
        LOG("LoRa TX failed: %d\n", state);
        return false;
    }
    LOG("LoRa TX ok (%u bytes)\n", (unsigned)sizeof(frame));
    return true;
}

#if DEBUG
void loraPrintFrame(const HydriaFrame &frame) {
    // Hex dump
    Serial.print("FRAME[");
    const uint8_t *b = reinterpret_cast<const uint8_t *>(&frame);
    for (size_t i = 0; i < sizeof(frame); i++) {
        if (b[i] < 0x10) Serial.print('0');
        Serial.print(b[i], HEX);
    }
    Serial.println("]");
    // Human-readable
    if (frame.sinceMeasureS == FRAME_NO_PRIOR_MEASURE) {
        Serial.printf("  wake=%-5s since=first-boot sonar=%.1f cm turb=%u hum=%u\n",
            (frame.flags & FRAME_FLAG_EXT_WAKEUP) ? "ext" : "timer",
            frame.sonarMm / 10.0f,
            frame.turbidity,
            frame.humidity);
    } else {
        Serial.printf("  wake=%-5s since=%u s sonar=%.1f cm turb=%u hum=%u\n",
            (frame.flags & FRAME_FLAG_EXT_WAKEUP) ? "ext" : "timer",
            frame.sinceMeasureS,
            frame.sonarMm / 10.0f,
            frame.turbidity,
            frame.humidity);
    }
}
#endif
