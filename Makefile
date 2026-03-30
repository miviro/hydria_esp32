FQBN      := esp32:esp32:esp32
PORT      := /dev/ttyUSB0
SKETCH    := .
BUILD_DIR := build

compile:
	arduino-cli compile --fqbn $(FQBN) --build-path $(BUILD_DIR) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) --input-dir $(BUILD_DIR) $(SKETCH)

serial:
	tio $(PORT)

all: clean compile upload serial

clean:
	rm -rf $(BUILD_DIR)

.PHONY: compile upload all clean
