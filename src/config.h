#pragma once

// Deep Sleep
#define SLEEP_INTERVAL_S 300 // 5 minutes

// Wakeup
#define WAKEUP_PIN GPIO_NUM_4
#define EXT_WAKEUP_THRESHOLD 20 // measure after this many external wakeups

// Device identifier (4 bits, 0–15) — packed into the low nibble of the header byte;
// the high nibble carries a 4-bit rolling counter that increments every transmit.
#define DEVICE_ID 0x1

// Readings
#define READINGS_PER_MEASUREMENT 5 // average over this many samples

// Sonar Pins
#define TRIGGER_PIN 48
#define ECHO_PIN 47

// Turbidity Pin
#define TURBIDITY_PIN 3  // ADC1_CH2, Header J3

// Humidity Pin
#define HUMIDITY_PIN 2   // ADC1_CH1, Header J3

// Heltec V4 OLED (SSD1306 128x64, I2C)
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define VEXT_CTRL 36 // active LOW - powers the OLED (and LoRa) via Vext rail

// Heltec V4 Battery ADC
#define VBAT_ADC_PIN  1  // ADC reads Vbat/2 through a 1:2 voltage divider
#define VBAT_ADC_CTRL 37 // active HIGH — enables the battery voltage divider

// LoRa SX1262 - internal SPI bus (Heltec V4)
#define LORA_SS 8
#define LORA_SCK 9
#define LORA_MOSI 10
#define LORA_MISO 11
#define LORA_RST 12
#define LORA_DIO1 14
#define LORA_BUSY 13
#define LORA_FREQ 868.0f
#define LORA_POWER 14 // dBm
