StellaX
=======

# Using Makefile
Targets:
    * compile
    * upload
    * clean
Variables:
    * ARDUINO_DIR - arduino directory (should use 1.8.1)
    * PORT - needed for uploading
           - may bew omited if still using the same port
Example:
    make upload ARDUINO_DIR=/opt/arduino PORT=/dev/ttyUSB0
