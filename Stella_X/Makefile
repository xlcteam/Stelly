CURR_DIR = $(shell pwd)

ifndef ARDUINO_DIR
$(error Please set ARDUINO_DIR to arduino directory you\'ve downloaded)
endif
ARDUINO = $(ARDUINO_DIR)/arduino
SKETCHBOOK = $(CURR_DIR)/sketchbook
PREFS_FILE = $(CURR_DIR)/.arduino_prefs.txt
BUILD_DIR = $(CURR_DIR)/build
MPU6050CAL_SKETCH = $(CURR_DIR)/i2cdevlib-submodule/Arduino/MPU6050/examples/IMU_Zero/IMU_Zero.ino

PORT_OPTION =
ifdef PORT
PORT_OPTION = --port $(PORT)
endif

ACTION_OPTION = --upload
ifdef COMPILE
ACTION_OPTION = --verify
endif

BOARD = "arduino:avr:mega:cpu=atmega2560"

define do_action
	$(ARDUINO) $(ACTION_OPTION)\
		--board $(BOARD)\
		$(PORT_OPTION)\
		--preferences-file $(PREFS_FILE)\
		--pref sketchbook.path=$(SKETCHBOOK)\
		--pref build.path=$(BUILD_DIR)\
		--preserve-temp-files\
		$(1)
endef


Stella_X:
	$(call do_action,$(CURR_DIR)/Stella_X/Stella_X.ino)

%.ino: FORCE
	$(call do_action,$(CURR_DIR)/$@)

MPU6050cal:
	$(eval ORIGINAL=$(shell grep -o Serial\.begin\([0-9]*\)\; $(MPU6050CAL_SKETCH)))
	sed -i 's/Serial\.begin([0-9]*);/Serial.begin(115200);/' $(MPU6050CAL_SKETCH)
	$(call do_action,$(MPU6050CAL_SKETCH))
	sed -i 's/Serial\.begin([0-9]*);/$(ORIGINAL)/' $(MPU6050CAL_SKETCH)

serial_monitor:
	picocom -b 115200 -c --imap lfcrlf --omap crlf,delbs $(PORT)

clean:
	rm -rf build/

FORCE:

.PHONY: Stella_X MPU6050cal serial_monitor clean
