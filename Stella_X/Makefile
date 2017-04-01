CURR_DIR = $(shell pwd)

ifndef ARDUINO_DIR
$(error Please set ARDUINO_DIR to arduino directory you\'ve downloaded)
endif
ARDUINO = $(ARDUINO_DIR)/arduino
SKETCHBOOK = $(CURR_DIR)/sketchbook
BUILD_DIR = $(CURR_DIR)/build
SRC_SKETCH = $(CURR_DIR)/Stella_X/Stella_X.ino

ifndef PORT
PORT_OPTION =
else
PORT_OPTION = "--port $(PORT)"
endif
BOARD = "arduino:avr:mega:cpu=atmega2560"

compile:
	$(ARDUINO) --verify\
		--board $(BOARD)\
		--pref sketchbook.path=$(SKETCHBOOK)\
		--pref build.path=$(BUILD_DIR)\
		--preserve-temp-files\
		$(SRC_SKETCH)

upload:
	$(ARDUINO) --upload\
		--board $(BOARD)\
		$(PORT_OPTION)\
		--pref sketchbook.path=$(SKETCHBOOK)\
		--pref build.path=$(BUILD_DIR)\
		--preserve-temp-files\
		$(SRC_SKETCH)

clean:
	rm -rf build/*

.PHONY: compile upload clean
