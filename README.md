# Requirements
```
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh`
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/3.3.8/package_heltec_esp32_index.json
arduino-cli core update-index --config-file ~/.arduino15/arduino-cli.yaml
arduino-cli core install esp32:esp32
arduino-cli core install Heltec-esp32:esp32
arduino-cli lib install "U8g2"
arduino-cli lib install RadioLib
```
# Usage
`make all`
Press `EN` button on ESP32 to run.
