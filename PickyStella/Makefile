CURR_DIR = $(shell pwd)

ifndef ARDUINO_DIR
$(error Please set ARDUINO_DIR to arduino directory you\'ve downloaded)
endif
ARDUINO = $(ARDUINO_DIR)/arduino
SKETCHBOOK = $(CURR_DIR)/sketchbook
PREFS_FILE = $(CURR_DIR)/.arduino_prefs.txt
BUILD_DIR = $(CURR_DIR)/build
ifndef SRC_SKETCH
SRC_SKETCH = $(CURR_DIR)/PickyStella/PickyStella.ino
endif

ifndef PORT
PORT_OPTION =
else
PORT_OPTION = --port $(PORT)
endif
BOARD = "arduino:avr:mega:cpu=atmega2560"

compile:
	$(ARDUINO) --verify\
		--board $(BOARD)\
		--preferences-file $(PREFS_FILE)\
		--pref sketchbook.path=$(SKETCHBOOK)\
		--pref build.path=$(BUILD_DIR)\
		--preserve-temp-files\
		$(SRC_SKETCH)

upload:
	$(ARDUINO) --upload\
		--board $(BOARD)\
		$(PORT_OPTION)\
		--preferences-file $(PREFS_FILE)\
		--pref sketchbook.path=$(SKETCHBOOK)\
		--pref build.path=$(BUILD_DIR)\
		--preserve-temp-files\
		$(SRC_SKETCH)

clean:
	rm -rf build/

.PHONY: compile upload clean
