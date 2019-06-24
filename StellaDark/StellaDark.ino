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
int spd_forward = 180;//180
int spd_forward_strong = 220;//220
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
	Serial.begin(9600);
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
	while (analogRead(A6) < 100) {
		if (analogRead(A7) > 100) {
			compass_set_north();
		}
		motors_off();
	}
	ws[0] = ws[1] = ws[2] = 0;
}

void loop() {
	touch_line = line_sensors_dir();
	if (touch_line == 255) {
		pixyViSy.update(PIXYVISY_TEAM);

		distance = team_distance();
		if (distance == PIXYVISY_NOBLOCK || IR_ATACK_VALUE > IR_V3_strength()){
			defend = 0;
			za_loptou();
		} else {
			defend = 1;
			defender();
		}
	} else {
		uint8_t dir_from_line = (touch_line + 4) % 8;
		switch (dir_from_line) {
		case 0:

			from_line(forward, true);
			break;

		case 1:

			from_line(right_forward, true);
			break;

		case 2:

			from_line(right, false);
			break;

		case 3:

			from_line(right_backward, false);
			break;

		case 4:

			from_line(backward, false);
			break;

		case 5:

			from_line(left_backward, false);
			break;

		case 6:

			from_line(vlavo, false);
			break;

		case 7:

			from_line(left_forward, true);
			break;
		}
	}
}

void from_line(void (* p_fun)(int16_t), bool back) {
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

void za_loptou() {
	defend = 0;
	if (analogRead(A6) > 100) {
		motors_off();
		return;
	}
	uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
	uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
	IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
	IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);

	if (dir_1 != 0 && dir_2 != 0) { // both sensors received the signal
		if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 >
				s1_2 + s3_2 + s5_2 + s7_2 + s9_2) {
			// stronger signal from IRSEEKER1
			switch (dir_1) {
			case 1:
				backward(spd);
				break;

			case 2:
				left_backward(spd);
				break;

			case 3:
				left_backward(spd);
				break;

			case 4:
				vlavo(spd);
				break;

			case 5:
				forward(spd);
				break;

			case 6:
				right(spd);
				break;

			case 7:
				right_backward(spd);
				break;

			case 8:
				backward(spd);
				break;

			case 9:
				backward(spd);
				break;

			default:
				stay_direct();
				break;
			}
		} else { // stronger signal from IRSEEKER2
			switch (dir_2) {
			case 1:
				right_backward(spd);
				break;

			case 2:
				right_backward(spd);
				break;

			case 3:
				backward(spd);
				break;

			case 4:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 5:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 6:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 7:
				backward(spd);
				break;

			case 8:
				left_backward(spd);
				break;

			case 9:
				left_backward(spd);
				break;

			default:
				stay_direct();
				break;
			}
		}
	} else {
		switch (dir_1) {
		case 1:
			backward(spd);
			break;

		case 2:
			left_backward(spd);
			break;

		case 3:
			left_backward(spd);
			break;

		case 4:
			vlavo(spd);
			break;

		case 5:
			forward(spd);
			break;

		case 6:
			right(spd);
			break;

		case 7:
			right_backward(spd);
			break;

		case 8:
			backward(spd);
			break;

		case 9:
			backward(spd);
			break;

		default:
			switch (dir_2) {
			case 1:
				right_backward(spd);
				break;

			case 2:
				right_backward(spd);
				break;

			case 3:
				backward(spd);
				break;

			case 4:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 5:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 6:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 7:
				backward(spd);
				break;

			case 8:
				left_backward(spd);
				break;

			case 9:
				left_backward(spd);
				break;

			default:
				stay_direct();
			}
		}
	}
}

void defender() {
	defend = 1;
	if (analogRead(A6) > 100) {
		motors_off();
		return;
	}
	uint8_t dir_1, s1_1, s3_1, s5_1, s7_1, s9_1;
	uint8_t dir_2, s1_2, s3_2, s5_2, s7_2, s9_2;
	IRseeker1(&dir_1, &s1_1, &s3_1, &s5_1, &s7_1, &s9_1);
	IRseeker2(&dir_2, &s1_2, &s3_2, &s5_2, &s7_2, &s9_2);

	if (dir_1 != 0 && dir_2 != 0) { // both sensors received the signal
		if (s1_1 + s3_1 + s5_1 + s7_1 + s9_1 >
				s1_2 + s3_2 + s5_2 + s7_2 + s9_2) {
			// strogner signal from IRSEEKER1
			switch (dir_1) {
			case 1:
				backward(spd);
				break;

			case 2:
				left_backward(spd);
				break;

			case 3:
				left_backward(spd);
				break;

			case 4:
				vlavo(spd);
				break;

			case 5:
				move_team_distance();
				break;

			case 6:
				right(spd);
				break;

			case 7:
				right_backward(spd);
				break;

			case 8:
				backward(spd);
				break;

			case 9:
				backward(spd);
				break;

			default:
				move_team_distance();
				break;
			}
		} else { // stronger signal from IRSEEKER2
			switch (dir_2) {
			case 1:
				right_backward(spd);
				break;

			case 2:
				right_backward(spd);
				break;

			case 3:
				backward(spd);
				break;

			case 4:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 5:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 6:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 7:
				backward(spd);
				break;

			case 8:
				left_backward(spd);
				break;

			case 9:
				left_backward(spd);
				break;

			default:
				stay_direct();
				break;
			}
		}
	} else {
		switch (dir_1) {
		case 1:
			backward(spd);
			break;

		case 2:
			left_backward(spd);
			break;

		case 3:
			left_backward(spd);
			break;

		case 4:
			left(spd);
			break;

		case 5:
			move_team_distance();
			break;

		case 6:
			right(spd);
			break;

		case 7:
			right_backward(spd);
			break;

		case 8:
			backward(spd);
			break;

		case 9:
			backward(spd);
			break;

		default:
			switch (dir_2) {
			case 1:
				right_backward(spd);
				break;

			case 2:
				right_backward(spd);
				break;

			case 3:
				backward(spd);
				break;

			case 4:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 5:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 6:
				if (s5_2 > 100) {
					right(spd);
				} else {
					backward(spd);
				}
				break;

			case 7:
				backward(spd);
				break;

			case 8:
				left_backward(spd);
				break;

			case 9:
				left_backward(spd);
				break;

			default:
				move_team_distance();
			}
		}
	}
}
