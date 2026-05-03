#include "lora.h"
#include "debug.h"
#include <RadioLib.h>
#include <SPI.h>
#include <mbedtls/md.h>

static const uint8_t HMAC_KEY[]  = "HYDRIA_ALMIN";
// Number of data bytes to authenticate (everything except the hmac field itself)
static const size_t  HMAC_DATA_LEN = sizeof(HydriaFrame) - sizeof(HydriaFrame::hmac);

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

bool loraSend(HydriaFrame &frame) {
    // Compute HMAC-SHA256 over the data fields and truncate to 4 bytes
    uint8_t full_hmac[32];
    mbedtls_md_hmac(
        mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
        HMAC_KEY, sizeof(HMAC_KEY) - 1,
        reinterpret_cast<const uint8_t *>(&frame), HMAC_DATA_LEN,
        full_hmac
    );
    memcpy(frame.hmac, full_hmac, sizeof(frame.hmac));

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
    // Human-readable (wake source inferred from count)
    const char *wake = (frame.wakeCount >= EXT_WAKEUP_THRESHOLD) ? "ext" : "timer";
    uint8_t id   = FRAME_DEVICE_ID(frame.header);
    uint8_t roll = FRAME_ROLLING(frame.header);

    if (frame.sonarMm == 0xFFFF) {
        Serial.printf("  id=%u roll=%u wake=%-5s wakes=%u sonar=no echo  turb=%u hum=%u batt=%u%%\n",
            id, roll, wake, frame.wakeCount, frame.turbidity, frame.humidity, frame.battery);
    } else {
        Serial.printf("  id=%u roll=%u wake=%-5s wakes=%u sonar=%.1f cm turb=%u hum=%u batt=%u%%\n",
            id, roll, wake, frame.wakeCount, frame.sonarMm / 10.0f, frame.turbidity, frame.humidity, frame.battery);
    }
    Serial.printf("  hmac=%02X%02X%02X%02X%02X%02X\n",
        frame.hmac[0], frame.hmac[1], frame.hmac[2],
        frame.hmac[3], frame.hmac[4], frame.hmac[5]);
}
#endif
