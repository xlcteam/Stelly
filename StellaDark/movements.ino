/**
 * functions for movements
 */
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
#define P_TEAM 5
#define P_TEAM_NEAR 18
//////////////////////////////////////////////////////
#define P 3;
#define I 0.07
#define D 10
#define D_change 3
//////////////////////////////////////////////////////
#define max_I 100*100
#define min_I -100*100
//////////////////////////////////////////////////////
#define BALL_P 0.7//1.5
#define BALL_P_DISTANCE 0.05
#define BALL_I 0
#define BALL_D 3//7
#define D_change_ball 2
//////////////////////////////////////////////////////

int PID(int16_t speeds[3], int setpoint) {
	static int input;
	static int out;
	static int error;
	static int last_input = 0;
	static long last_time = 0;
	static int last_error = 0;
	static long now = 0;
	static int error_index = -1;
	static int errors[D_change] = { 0 };

	input = robot_angle();
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

	error_index = (error_index + 1) % D_change;
	errors[error_index] = error;

	now = millis();
	if (error != 0) {
		PID_P = error * P;
		PID_I += (now - last_time) * error * I ;
		PID_I = constrain(PID_I, min_I, max_I);
		PID_D = (errors[(D_change - 1) - error_index] - error) * D;

		out = PID_P + (PID_I / 100) - PID_D;
		out = constrain(out, minimal_value, maximal_value);
	} else {
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
///////////////////////////////////////////////////////////////

int centering_to_ball() {
	static int error_index_ball = 0;
	static int errors_ball[D_change_ball] {0};
	static long	last_time_ball = 0;
	static int	last_error_ball = 0;
	static int	last_input_ball = 0;
	static int PID_P_ball = 0;
	static long PID_I_ball = 0;
	static int PID_D_ball = 0;
	int spd;
	int error_ball;
	int now_ball;
	int out_ball;
	int strength;
	error_ball = IR_V3();
	strength = IR_V3_strength();
	if (error_ball < 10 && error_ball > -10) {
		spd = spd_forward_strong;
	} else {
		spd = spd_forward;
	}
	int maximal_value_ball = 255 - spd ;
	int minimal_value_ball = -255 + spd ;
	error_index_ball = (error_index_ball + 1) % D_change_ball;
	errors_ball[error_index_ball] = error_ball;

	now_ball = millis();
	if (error_ball != 0) {
		PID_P_ball = (error_ball * BALL_P) * (BALL_P_DISTANCE * strength);
		PID_I_ball += ((now_ball - last_time_ball) * error_ball ) * BALL_I ;
		PID_I_ball = constrain(PID_I_ball, min_I, max_I);
		PID_D_ball = (errors_ball[(D_change_ball - 1) - error_index_ball] - error_ball) * BALL_D;

		out_ball = PID_P_ball + (PID_I_ball / 100) - PID_D_ball;
		out_ball = constrain(out_ball, minimal_value_ball, maximal_value_ball);
	} else {
		PID_P_ball = 0;
		PID_I_ball = 0;
		PID_D_ball = 0;
		out_ball = 0;
	}

	last_time_ball = now_ball;
	last_error_ball = error_ball;
	last_input_ball = error_ball;

	return out_ball;
}

// direct means without rotating; "spds" should be an array of size 3
void move_direct(int16_t *spds) {
	int compensation = PID(spds, 0);
	motorA.go(spds[0] + compensation);
	motorB.go(spds[1] + compensation);
	motorC.go(spds[2] + compensation);
}

void forward(int16_t spd) {
	int compensation = centering_to_ball();
	int16_t speeds[3] = {spd + compensation / 2,
		-spd - compensation / 2, -compensation};
	motion_last_dir = 0;
	move_direct(speeds);
}

void right_forward(int16_t spd) {
	int16_t speeds[3] = {spd, 0, -spd};
	motion_last_dir = 1;
	move_direct(speeds);
}

void right(int16_t spd) {
	int16_t speeds[3] = {spd / 2, spd / 2, -spd};
	motion_last_dir = 2;
	move_direct(speeds);
}

void right_backward(int16_t spd) {
	int16_t speeds[3] = {0, spd, -spd};
	motion_last_dir = 3;
	move_direct(speeds);
}

void backward(int16_t spd) {
	int16_t speeds[3] = { -spd, spd, 0};
	motion_last_dir = 4;
	move_direct(speeds);
}

void left_backward(int16_t spd) {
	int16_t speeds[3] = { -spd, 0, spd};
	motion_last_dir = 5;
	move_direct(speeds);
}

void left(int16_t spd) {
	int16_t speeds[3] = { -spd / 2, -spd / 2, spd};
	motion_last_dir = 6;
	move_direct(speeds);
}

void left_forward(int16_t spd) {
	int16_t speeds[3] = {0, -spd, spd};
	motion_last_dir = 7;
	move_direct(speeds);
}

void stay_direct() {
	int16_t speeds[3] = {0, 0, 0};
	move_direct(speeds);
}

void motors_off() {
	motorA.go(0);
	motorB.go(0);
	motorC.go(0);
}

void move_team_distance() {
	if (TEAM_DISTANCE + TEAM_DISTANCE_TOLERATION < distance) {
		// too far away
		forward((distance - TEAM_DISTANCE) * P_TEAM);
	} else if (TEAM_DISTANCE - TEAM_DISTANCE_TOLERATION > distance) {
		// too close
		backward((TEAM_DISTANCE - distance) * P_TEAM_NEAR);
	} else {
		// OK
		stay_direct();
	}
}
