#include "display.h"
#include "config.h"
#include <U8g2lib.h>
#include <Wire.h>

static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_RST, OLED_SCL,
                                                OLED_SDA);

void displayBegin() {
    u8g2.begin();
}

void displayStatus(const char *text) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso28_tr);
    int16_t w = u8g2.getStrWidth(text);
    int16_t x = (128 - w) / 2;
    int16_t y = (64 + 28) / 2; // baseline at vertical center
    u8g2.drawStr(x, y, text);
    u8g2.sendBuffer();
}

void displayReadings(float sonarCm, int turbidity, int humidity, int batteryPct, uint16_t wakeCount) {
    char buf[40];
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_logisoso16_tr);
    if (sonarCm < 0) snprintf(buf, sizeof(buf), "S: ---");
    else              snprintf(buf, sizeof(buf), "S: %.1f cm", sonarCm);
    u8g2.drawStr(0, 16, buf);
    snprintf(buf, sizeof(buf), "T: %d", turbidity);
    u8g2.drawStr(0, 32, buf);
    snprintf(buf, sizeof(buf), "H: %d", humidity);
    u8g2.drawStr(0, 48, buf);

    u8g2.setFont(u8g2_font_6x10_tr);
    snprintf(buf, sizeof(buf), "B: %d%%   W: %u", batteryPct, (unsigned)wakeCount);
    u8g2.drawStr(0, 60, buf);

    u8g2.sendBuffer();
}

void displayOff() {
    u8g2.setPowerSave(1);
}
