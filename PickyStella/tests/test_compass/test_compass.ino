#include <RelativeHMC6352.h>
#include <Motor.h>
#include "pinout.h"

void setup()
{
    Serial.begin(115200);
    compass_set_north();
}

void loop()
{
    uint32_t time = millis();
    if ((time / 1000) % 4 == 0) {
        move_up(255);
    } else if ((time / 1000) % 4 == 1) {
        move_left(255);
    } else if ((time / 1000) % 4 == 2) {
        move_right(255);
    } else {
        move_back(255);
    }
    Serial.println(compass_angle());
}
