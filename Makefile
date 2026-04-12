FQBN      := Heltec-esp32:esp32:heltec_wifi_lora_32_V4
PORT      := /dev/ttyACM0
SKETCH    := .
BUILD_DIR := build

compile:
	arduino-cli compile --fqbn $(FQBN) --build-property build.extra_flags=-DARDUINO_USB_CDC_ON_BOOT=1 --build-path $(BUILD_DIR) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) --input-dir $(BUILD_DIR) $(SKETCH)

serial:
	tio $(PORT)

all: clean compile upload serial

clean:
	rm -rf $(BUILD_DIR)

.PHONY: compile upload all clean
