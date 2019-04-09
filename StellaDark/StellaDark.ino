#include <Wire.h>
#include <Motor.h>

#define LINE_SENSORS_COUNT 3
#define LINE_FRONT_PIN A0
#define LINE_LEFT_PIN A2
#define LINE_RIGHT_PIN A1
#define LINE_THRESH_PIN 11

#define LINE_STRONG_TIME 80
#define LINE_TIME 300
#define LINE_BACK_TIME 400 // aplied for 3, 4, 5
#define STOP_LINE_TIME 0
#define MUTEX(state) mutex = (state)
#define LINE_USE_INT 1
#define LINE_THRESH 150

uint8_t line_use_int = LINE_USE_INT;
uint8_t light_pwm = LINE_THRESH;
uint8_t motion_last_dir;

uint8_t volatile ws[] = {0, 0, 0};
uint8_t mutex = 0;
uint8_t touch_line;
uint32_t start;
uint8_t touch_line_dir;

int spd = 150;
int line_spd = 80;
int line_strong_spd = 250;

ISR(PCINT1_vect)
{
    if (!mutex) {
        if (!ws[0] && read_line_sensor(0)) {
            ws[0] = 1;
        }

        if (!ws[1] && read_line_sensor(1)) {
            ws[1] = 1;
        }

        if (!ws[2] && read_line_sensor(2)) {
            ws[2] = 1;
        }
    }
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    setup_line_sensors();

    pinMode(A7, INPUT);
    pinMode(A6, INPUT);
    while (analogRead(A6) < 100) {
        if (analogRead(A7) > 100) {
            compass_set_north();
        }
    }
    ws[0] = ws[1] = ws[2] = 0;
}

void loop()
{
    touch_line = line_sensors_dir();
    if (touch_line == 255) {
        za_loptou();
    } else {
        uint8_t dir_from_line = (touch_line + 4) % 8;
        switch (dir_from_line) {
        case 0:
            /* vpred(spd);
            delay(LINE_TIME);*/

            /* IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
            touch_line_dir = dir;
            while (touch_line_dir == dir) {
            IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
            }*/
            from_line(vpred, true);
            break;

        case 1:
            /* vpravo_vpred(line_spd);
            delay(LINE_TIME);*/

            /*IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
            touch_line_dir = dir;
            while (touch_line_dir == dir) {
            IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
            }*/
            from_line(vpravo_vpred, true);
            break;

        case 2:
            /* vpravo(line_spd);
            delay(LINE_TIME);*/

            /*  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            touch_line_dir = dir;
            while (touch_line_dir == dir) {
            IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
            }*/
            from_line(vpravo, false);
            break;

        case 3:
            /*vpravo_vzad(line_spd);
            delay(LINE_TIME);
            */
            /*
                IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
            from_line(vpravo_vzad, false);
            break;

        case 4:
            /*  vzad(line_spd);
            delay(LINE_TIME);
            */
            /*
                IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
            from_line(vzad, false);
            break;

        case 5:
            /*vlavo_vzad(line_spd);
            delay(LINE_TIME);
            */

            /*  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            touch_line_dir = dir;
            while (touch_line_dir == dir) {
            IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
            }*/
            from_line(vlavo_vzad, false);
            break;

        case 6:
            /* vlavo(line_spd);
            delay(LINE_TIME);*/

            /*   IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            touch_line_dir = dir;
            while (touch_line_dir == dir) {
             IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
             motors_off();
            }*/
            from_line(vlavo, false);
            break;

        case 7:
            /* vlavo_vpred(line_spd);
            delay(LINE_TIME);*/
            /*
                IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
            from_line(vlavo_vpred, true);
            break;
        }
    }
}

void from_line(void (* p_fun)(int16_t), bool back)
{
    start = millis();
    uint32_t delay_time = back ? LINE_BACK_TIME : LINE_TIME;
    while (millis() - start < LINE_STRONG_TIME) {
        p_fun(line_strong_spd);
    }
    while (millis() - start < delay_time) {
        p_fun(line_spd);
    }
    start = millis();
    do {
        motors_off();
    } while (millis() - start < STOP_LINE_TIME);
}

void za_loptou()
{
    if (analogRead(A6) > 100) {
        motors_off();
        return;
    }
    uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
    uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
    IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
    IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);

    if (dir_1 != 0 && dir_2 != 0) { // at least one sensor received the signal
        if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 >
        s1_2 + s3_2 + s5_2 + s7_2 + s9_2) { // strogner signal from IRSEEKER1
            switch (dir_1) {
            case 1:
                // Serial.println("11");
                vzad(spd);
                break;

            case 2:
                // Serial.println("12");
                vlavo_vzad(spd);
                break;

            case 3:
                //   Serial.println("13");
                vlavo_vzad(spd);
                break;

            case 4:
                //    Serial.println("14");
                vlavo(spd);
                break;

            case 5:
                //    Serial.println("15");
                vpred(spd);
                break;

            case 6:
                //  Serial.println("16");
                vpravo(spd);
                break;

            case 7:
                //    Serial.println("17");
                vpravo_vzad(spd);
                break;

            case 8:
                //      Serial.println("18");
                vzad(spd);
                break;

            case 9:
                //     Serial.println("19");
                vzad(spd);
                break;

            default:
                //     Serial.println("00");
                na_mieste();
                break;
            }
        } else { // stronger signal from IRSEEKER2
            switch (dir_2) {
            case 1:
                //  Serial.println("21");
                vpravo_vzad(spd);
                break;

            case 2:
                //   Serial.println("22");
                vpravo_vzad(spd);
                break;

            case 3:
                //   Serial.println("23");
                vzad(spd);
                break;

            case 4:
                //Serial.println("24");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 5:
                // Serial.println("25");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 6:
                //Serial.println("26");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 7:
                //  Serial.println("27");
                vzad(spd);
                break;

            case 8:
                //  Serial.println("28");
                vlavo_vzad(spd);
                break;

            case 9:
                // Serial.println("29");
                vlavo_vzad(spd);
                break;

            default:
                // Serial.println("00");
                na_mieste();
                break;
            }
        }
    } else {
        switch (dir_1) {
        case 1:
            // Serial.println("11");
            vzad(spd);
            break;

        case 2:
            // Serial.println("12");
            vlavo_vzad(spd);
            break;

        case 3:
            //Serial.println("13");
            vlavo_vzad(spd);
            break;

        case 4:
            // Serial.println("14");
            vlavo(spd);
            break;

        case 5:
            //Serial.println("15");
            vpred(spd);
            break;

        case 6:
            // Serial.println("16");
            vpravo(spd);
            break;

        case 7:
            //Serial.println("17");
            vpravo_vzad(spd);
            break;

        case 8:
            // Serial.println("18");
            vzad(spd);
            break;

        case 9:
            //Serial.println("19");
            vzad(spd);
            break;

        default:
            switch (dir_2) {
            case 1:
                //Serial.println("21");
                vpravo_vzad(spd);
                break;

            case 2:
                // Serial.println("22");
                vpravo_vzad(spd);
                break;

            case 3:
                // Serial.println("23");
                vzad(spd);
                break;

            case 4:
                //  Serial.println("24");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 5:
                // Serial.println("25");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 6:
                // Serial.println("26");
                if (s5_2 > 100) {
                    vpravo(spd);
                } else {
                    vzad(spd);
                }
                break;

            case 7:
                // Serial.println("27");
                vzad(spd);
                break;

            case 8:
                //Serial.println("28");
                vlavo_vzad(spd);
                break;

            case 9:
                // Serial.println("29");
                vlavo_vzad(spd);
                break;

            default:
                // Serial.println("00");
                na_mieste();
            }
        }
    }
}
