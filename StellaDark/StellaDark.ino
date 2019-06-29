/**
 * XLC TEAM Robocup 2019 Sydney
 * Soccer light weight
 * Šimon Varga
 * Jakub Gál
 */
#include <Wire.h>
#include <Motor.h>
#include <PixyViSy.h>
#include <SPI.h>
#include <Pixy.h>
#include <XLCPixy.h>
#include <stdint.h>

#define DEBUG 0
#define TEST test_light_sensors

#define FOC_LEN_X 266
#define FOC_LEN_Y 237

#define GOAL_SIG 1
#define BALL_SIG 2
#define COLOR_CODE_SIG 55 //in decimal 

#define MIN_GOAL_AREA 100
#define GOAL_ANGLE_NO_GOAL 255
#define MAX_NO_TEAM_TIME 1000
#define TEAM_DISTANCE 30
#define TEAM_DISTANCE_TOLERATION 5
#define IR_ATACK_VALUE 25
PixyViSy pixyViSy(FOC_LEN_X, FOC_LEN_Y);

#define LINE_SENSORS_COUNT 3
#define LINE_FRONT_PIN A0
#define LINE_LEFT_PIN A2
#define LINE_RIGHT_PIN A1
#define LINE_THRESH_PIN 11

#define LINE_STRONG_TIME 80
#define LINE_TIME 300
#define LINE_BACK_TIME 400 // aplied for line_dir 3, 4, 5
#define STOP_LINE_TIME 0
#define MUTEX(state) mutex = (state)
#define LINE_USE_INT 1
#define LINE_THRESH 255

#define SPD 150
#define LINE_SPD 80
#define LINE_STRONG_SPD 250
#define SPD_FORWARD 180
#define SPD_FORWARD_STRONG 220

uint8_t line_use_int = LINE_USE_INT;
uint8_t light_pwm = LINE_THRESH;
uint8_t motion_last_dir;

uint8_t volatile ws[] = {0, 0, 0};
uint8_t mutex = 0;
uint8_t touch_line;
uint32_t start;
uint8_t touch_line_dir;

bool defend = 0;
int16_t last_distance;
int16_t distance;

ISR(PCINT1_vect) {
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

void setup() {
	Serial.begin(115200);
	Wire.begin();
	setup_line_sensors();
	compass_set_north();
	pixyViSy.setGoalSig(GOAL_SIG);
	pixyViSy.setBallSig(BALL_SIG);
	pixyViSy.setTeamSig(COLOR_CODE_SIG);
	pixyViSy.setTeamSize(5);
	pixyViSy.setMinGoalArea(MIN_GOAL_AREA);
	pinMode(A7, INPUT);
	pinMode(A6, INPUT);
#if DEBUG == 0
	while (analogRead(A6) < 100) {
		if (analogRead(A7) > 100) {
			compass_set_north();
		}
		motors_off();
	}
#else
	Serial.println("Test");
	motors_off();
#endif
	ws[0] = ws[1] = ws[2] = 0;
}

void loop() {
#if DEBUG == 0
	if (analogRead(A6) > 100) {
		motors_off();
		return;
	}

	touch_line = line_sensors_dir();
	if (touch_line == 255) {
		pixyViSy.update(PIXYVISY_TEAM);

		distance = team_distance();
		if (distance == PIXYVISY_NOBLOCK || IR_ATACK_VALUE > IR_V3_strength()){
			defend = 0;
			attacker();
		} else {
			defend = 1;
			defender();
		}
	} else {
		from_line(touch_line);
	}
#else
	TEST();
#endif
}


void attacker() {
	uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
	uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
	IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
	IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);

	if (dir_1 && dir_2) { // both sensors received the signal
		if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 >
				s1_2 + s3_2 + s5_2 + s7_2 + s9_2) {
			move_by_front_IR(dir_1, SPD);
		} else { // stronger signal from IRSEEKER2
			move_by_back_IR(dir_2, SPD, s5_2 > 100 ? true : false);
		}
	} else if (dir_1) {
		move_by_front_IR(dir_1, SPD);
	} else {
		move_by_back_IR(dir_2, SPD, s5_2 > 100 ? true : false);
	}
}

void defender() {
	uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
	uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
	IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
	IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);

	if (dir_1 && dir_2) { // both sensors received the signal
		if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 >
				s1_2 + s3_2 + s5_2 + s7_2 + s9_2) {
			move_by_front_IR(dir_1, SPD);
		} else { // stronger signal from IRSEEKER2
			move_by_back_IR(dir_2, SPD, s5_2 > 100 ? true : false);
		}
	} else if (dir_1) {
		if (dir_1 == 5) {
			move_team_distance();
		} else {
			move_by_front_IR(dir_1, SPD);
		}
	} else if (dir_2) {
		move_by_back_IR(dir_2, SPD, s5_2 > 100 ? true : false);
	} else {
		move_team_distance();
	}
}
