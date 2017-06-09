#ifndef PINOUT_H
#define PINOUT_H

#include <Arduino.h>

#define BTN1_PIN 27
#define BTN2_PIN 25
#define BTN3_PIN 24
#define BTN4_PIN 37

#define LED1_PIN 47
#define LED2_PIN 49
#define LED3_PIN 43
#define LED4_PIN 45

#define LCD_RS_PIN 32
#define LCD_EN_PIN 33
#define LCD_D4_PIN 35
#define LCD_D5_PIN 29
#define LCD_D6_PIN 30
#define LCD_D7_PIN 31

/* IR sensors marked clockwise */
#define IR1_PIN A10
#define IR2_PIN A11
#define IR3_PIN A12
#define IR4_PIN A13
#define IR5_PIN A14
#define IR6_PIN A15
#define IR7_PIN A0
#define IR8_PIN A1
#define IR9_PIN A2
#define IR10_PIN A3
#define IR11_PIN A7
#define IR_BOTTOM1_PIN A6
#define IR_BOTTOM2_PIN A5
#define IR_BOTTOM3_PIN A4

/* Warning: When changing, also edit line_sensors.ino !!! */
#define LINE_S_FRONT_PIN 13 // (PCINT7, PB7)
#define LINE_S_LEFT1_PIN 12 // the rightmost (PCINT6, PB6)
#define LINE_S_LEFT2_PIN 15 // (PCINT9, PJ0)
#define LINE_S_RIGHT1_PIN 11 // the leftmost (PCINT5, PB5)
#define LINE_S_RIGHT2_PIN 14 // (PCINT10, PJ1)
#define LINE_S_BACK_PIN 10 // (PCINT4, PB4)
#define LINE_S_THRESH_PIN 9

#define DRIBBLER_SENSOR_PIN 44
#define DRIBBLER_PIN 28

#define KICKER_PIN 26

#define MOTOR_A_DIR_PIN 46
#define MOTOR_B_DIR_PIN 23
#define MOTOR_C_DIR_PIN 41
#define MOTOR_D_DIR_PIN 48
#define MOTOR_A_PWM_PIN 8
#define MOTOR_B_PWM_PIN 2
#define MOTOR_C_PWM_PIN 3
#define MOTOR_D_PWM_PIN 7

#endif /* PINOUT_H */
