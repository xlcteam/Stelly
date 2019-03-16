#include <Wire.h>
#include <Motor.h>

#define TCAADDR 0x70

#define LINE_SENSORS_COUNT 3
#define LINE_FRONT_PIN A0
#define LINE_LEFT_PIN A2
#define LINE_RIGHT_PIN A1
#define LINE_THRESH_PIN 11

#define LINE_TIME 400
#define STOP_LINE_TIME 20
#define MUTEX(state) mutex[0] = mutex[1] = mutex[2] = state
#define LINE_MAX_DIFF_TIME 20000
#define LINE_USE_INT 1
#define LINE_THRESH 150
uint8_t line_use_int = LINE_USE_INT;
uint8_t light_pwm = LINE_THRESH;
uint8_t motion_last_dir;


uint32_t volatile ws_tmp[] = {0, 0, 0};
uint32_t ws[] = {0, 0, 0};
uint8_t mutex[] = {0, 0, 0};
uint8_t touch_line;
uint32_t start;
uint8_t touch_line_dir;


#define TCAADDR 0x70
#define MOTOR_RIGHT_DIR_PIN 8
#define MOTOR_RIGHT_PWM_PIN 10
#define MOTOR_LEFT_DIR_PIN 7
#define MOTOR_LEFT_PWM_PIN 9
#define MOTOR_BACK_DIR_PIN 4
#define MOTOR_BACK_PWM_PIN 6
Motor motorA = Motor(MOTOR_RIGHT_DIR_PIN, MOTOR_RIGHT_PWM_PIN);
Motor motorB = Motor(MOTOR_LEFT_DIR_PIN, MOTOR_LEFT_PWM_PIN);
Motor motorC = Motor(MOTOR_BACK_DIR_PIN, MOTOR_BACK_PWM_PIN);

int PID_P = 0;
long PID_I = 0;
int PID_D = 0;
//////////////////////////////////////////////////////
#define P 3;
#define I 0.07//0.008;
#define D 10//10;
#define D_change 3
//////////////////////////////////////////////////////
#define max_I 100*100
#define min_I -100*100
//////////////////////////////////////////////////////
int spd = 150;
//////////////////////////////////////////////////////

int x = 0;
int input;
int setpoint = 0;
int out;
int error;
int last_input = 0;
long last_time = 0;
int last_error = 0;
int compensation;
long now = 0;
int error_index = -1;
int errors[D_change] {0, 0, 0};
ISR(PCINT1_vect)
{
  if (!mutex[0] && !ws_tmp[0] && read_line_sensor(0)) {
    ws_tmp[0] = micros();
  }

  if (!mutex[1] && !ws_tmp[1] && read_line_sensor(1)) {
    ws_tmp[1] = micros();
  }

  if (!mutex[2] && !ws_tmp[2] && read_line_sensor(2)) {
    ws_tmp[2] = micros();
  }
}

int PID(int16_t speeds[3], int setpoint) {
  input = compass();
  if ( input > 180 || input < -180) {
    input = last_input;
  }
  int16_t maximal_value = -1000;
  int16_t minimal_value = 1000;
  for (int16_t i = 0; i < 3; i++) {
    if (speeds[i] < minimal_value) {
      minimal_value = speeds[i];
    }
    if (maximal_value < speeds[i])
      maximal_value = speeds[i];
  }
  maximal_value = 255 - maximal_value;
  minimal_value = -255 - minimal_value;



  error = setpoint - input;

  error_index++;
  if (error_index ==  D_change) {
    error_index = 0;
  }
  errors[error_index] = error;

  now = millis();
  if (error != 0) {
    PID_P = error * P;
    if (PID_I + ((now - last_time) * error ) * I  < min_I) {
      PID_I = min_I;
    }
    else if (PID_I + ((now - last_time) * error ) * I > max_I ) {
      PID_I = max_I;
    }
    else {
      PID_I += ((now - last_time) * error ) * I ;
    }
    PID_D = (errors[ (D_change - 1) - error_index] - error) * D;

    out = PID_P + (PID_I / 100) - PID_D;
    if (out > maximal_value) {
      out = maximal_value;
    }
    else if (out < minimal_value) {
      out = minimal_value;
    }

  }
  else {
    PID_P = 0;
    PID_I = 0;
    PID_D = 0;
    out = 0;

  }
  last_time = now;
  last_error = error;
  last_input = input;

  return out;
}
/////////
void vpred(int16_t spd) {
  int16_t speeds[3] = {spd, -spd, 0};

  motion_last_dir = 0;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vpravo_vpred(int16_t spd) {
  int16_t speeds[3] = {spd, 0, -spd};

  motion_last_dir = 1;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vpravo(int16_t spd) {
  int16_t speeds[3] = {spd / 2, spd / 2, -spd};

  motion_last_dir = 2;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}
void vpravo_vzad(int16_t spd) {
  int16_t speeds[3] = {0, spd, -spd};

  motion_last_dir = 3;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vzad(int16_t spd) {
  int16_t speeds[3] = { -spd, spd, 0};

  motion_last_dir = 4;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vlavo_vzad(int16_t spd) {
  int16_t speeds[3] = { -spd, 0, spd};

  motion_last_dir = 5;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vlavo(int16_t spd) {
  int16_t speeds[3] = { -spd / 2, -spd / 2, spd};

  motion_last_dir = 6;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void vlavo_vpred(int16_t spd) {
  int16_t speeds[3] = {0, -spd, spd};

  motion_last_dir = 7;
  compensation  = PID(speeds, 0);
  motorA.go(speeds[0] + compensation );
  motorB.go(speeds[1] + compensation );
  motorC.go(speeds[2] + compensation );
}

void na_mieste() {
  int16_t speeds[3] = {0, 0, 0};
  compensation = PID(speeds, 0);
  // //Serial.println(compensation);
  motorA.go(speeds[0] + compensation);
  motorB.go(speeds[1] + compensation);
  motorC.go(speeds[2] + compensation);
}
void motors_off() {
  motorA.go(0);
  motorB.go(0);
  motorC.go(0);
}
void za_loptou() {
  if (analogRead(A6) > 100) {
    motors_off();
  }
  else {
    uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
    uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
    IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
    IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);
    if (dir_1 != 0 && dir_2 != 0) {
      if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 > s1_2 + s3_2 + s5_2 + s7_2 + s9_2)
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
      else {

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
            }
            else {
              vzad(spd);
            }
            break;

          case 5:
            // Serial.println("25");
            if (s5_2 > 100) {
              vpravo(spd);
            }
            else {
              vzad(spd);
            }
            break;

          case 6:
            //Serial.println("26");
            if (s5_2 > 100) {
              vpravo(spd);
            }
            else {
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
    }
    else {

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
              }
              else {
                vzad(spd);
              }
              break;

            case 5:
              // Serial.println("25");
              if (s5_2 > 100) {
                vpravo(spd);
              }
              else {
                vzad(spd);
              }
              break;

            case 6:
              // Serial.println("26");
              if (s5_2 > 100) {
                vpravo(spd);
              }
              else {
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
}
void setup() {

  Serial.begin(9600);
  Wire.begin();
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);
  while (analogRead(A6) < 100) {
    if (analogRead(A7) > 100) {
      compass_set_north();
    }
    motors_off();
    ws[0] = 0;
    ws[1] = 0;
    ws[2] = 0;
  }

  //Serial.println("//////////////////////////////////////Start///////////////////////////////");
}

void loop() {

  touch_line = line_sensors_dir();
  if (line_sensors_dir() == 255) {
    za_loptou();
  }
  else {
    uint8_t dir, s1, s3, s5, s7, s9;
    switch (touch_line) {

      case 0:
        /* vpred(spd);
          delay(LINE_TIME);*/

        /* IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
          touch_line_dir = dir;
          while (touch_line_dir == dir) {
           IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
           motors_off();
          }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vpred(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 1:
        /* vpravo_vpred(spd);
          delay(LINE_TIME);*/

        /*IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
          touch_line_dir = dir;
          while (touch_line_dir == dir) {
          IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
          motors_off();
          }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vpravo_vpred(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 2:
        /* vpravo(spd);
          delay(LINE_TIME);*/

        /*  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
          touch_line_dir = dir;
          while (touch_line_dir == dir) {
            IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
          }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vpravo(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 3:
        /*vpravo_vzad(spd);
          delay(LINE_TIME);
        */
        /*
                IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vpravo_vzad(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 4:
        /*  vzad(spd);
          delay(LINE_TIME);
        */
        /*
                IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vzad(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 5:
        /*vlavo_vzad(spd);
          delay(LINE_TIME);
        */

        /*  IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
          touch_line_dir = dir;
          while (touch_line_dir == dir) {
            IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
            motors_off();
          }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vlavo_vzad(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 6:
        /* vlavo(spd);
          delay(LINE_TIME);*/

        /*   IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
           touch_line_dir = dir;
           while (touch_line_dir == dir) {
             IRseeker1(&dir, &s1, &s3, &s5, &s7, &s9);
             motors_off();
           }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vlavo(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }

        break;

      case 7:
        /* vlavo_vpred(spd);
          delay(LINE_TIME);*/
        /*
                IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
                touch_line_dir = dir;
                while (touch_line_dir == dir) {
                  IRseeker2(&dir, &s1, &s3, &s5, &s7, &s9);
                  motors_off();
                }*/
        start = millis();
        while ((millis() - start >= LINE_TIME) == 0) {
          vlavo_vpred(spd);
        }
        start = millis();
        while ((millis() - start >= STOP_LINE_TIME) == 0) {
          motors_off();
        }


        break;

    }

  }

  //test_sensors();
}
