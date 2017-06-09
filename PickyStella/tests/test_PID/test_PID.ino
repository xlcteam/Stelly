#include <vic.h>
#include <RelativeHMC6352.h>
#include <Motor.h>
#include "pinout.h"

/* not used, just because of errors: '...' was not declared */
#define ROTATE_SPEED 1
#define TO_NORTH_THRESH 1

#define kP 300 // percent
#define kI 300 // percent
#define kD 0

#define PLOT_MIN -260
#define PLOT_MAX 260

uint32_t last_time = 0;
int32_t P_term = 0;
int32_t I_term = 0;
int32_t D_term = 0;

int16_t spds[] = { 150, -150, -150, 150 };

void setup()
{
    Serial.begin(115200);
    Serial.print("min");
    Serial.print(" ");
    Serial.print("max");
    Serial.print(" ");
    Serial.print("error");
    Serial.print(" ");
    Serial.print("P");
    Serial.print(" ");
    Serial.print("I");
    Serial.print(" ");
    Serial.print("D");
    Serial.print(" ");
    Serial.println("output");

    setup_compass();
    compass_set_north();

    last_time = micros();
}

/*
d_time should not exceed 5 seconds
*/
void loop()
{
    static uint32_t change_set_point = millis();
    if (millis() - change_set_point > 3000) {
        compass_set_north_val((compass_north() - 90) % 360);
        change_set_point = millis();
    }

    int32_t min_limit = -1000;
    int32_t max_limit = 1000;
    for (uint8_t i = 0; i < 4; i++) {
        if (255 - spds[i] < max_limit) {
            max_limit = 255 - spds[i];
        }
        if (-255 - spds[i] > min_limit) {
            min_limit = -255 - spds[i];
        }       
    }

    int32_t error = (int32_t)compass_angle();
    if (error >= 180) {
        error -= 360;
    }
    int32_t d_time = micros() - last_time;

    P_term = error * kP; // [error]%
    int32_t compensation = (P_term + D_term) / 100;

    if (compensation > max_limit) {
        compensation = max_limit;
        I_term = 0;
    } else if (compensation < min_limit) {
        compensation = min_limit;
        I_term = 0;
    } else {
        I_term += error * d_time / 1000 * kI / 1000; // [error]% * mS
        compensation += I_term / 100;

        if (compensation > max_limit) {
            I_term -= (compensation - max_limit) * 100;
            compensation = max_limit;
        } else if (compensation < min_limit) {
            I_term += (min_limit - compensation) * 100;
            compensation = min_limit;
        }
    }

    last_time = micros();

    int16_t output_spds[4];
    output_spds[0] = spds[0] - compensation;
    output_spds[1] = spds[1] - compensation;
    output_spds[2] = spds[2] - compensation;
    output_spds[3] = spds[3] - compensation;
    move_raw(output_spds);

    Serial.print(PLOT_MIN);
    Serial.print(" ");
    Serial.print(PLOT_MAX);
    Serial.print(" ");
    Serial.print(error);
    Serial.print(" ");
    Serial.print(P_term / 100);
    Serial.print(" ");
    Serial.print(I_term / 100);
    Serial.print(" ");
    Serial.print(D_term);
    Serial.print(" ");
    Serial.println(compensation);
}
