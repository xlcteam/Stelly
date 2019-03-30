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
// TODO What about using "static"
int input;
int out;
int error;
int last_input = 0;
long last_time = 0;
int last_error = 0;
long now = 0;
int error_index = -1;
int errors[D_change] = { 0 };

int PID(int16_t speeds[3], int setpoint) {
  input = compass();
  if ( input > 180 || input < -180) { // TODO Is it possible?
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

  error = setpoint - input; // TODO Couldn't we do this with compass_set_north?

  error_index = (error_index + 1) % D_change;
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
    /* TODO Alternative - perhaps more straightforward
    PID_I += ((now - last_time) * error ) * I ;
    if (PID_I < min_I) {
        PID_I = min_I;
    } else if (PID_I > max_I) {
        PID_I = max_I;
    }
    */
    PID_D = (errors[(D_change - 1) - error_index] - error) * D;

    out = PID_P + (PID_I / 100) - PID_D;
    if (out > maximal_value) {
      out = maximal_value;
    } else if (out < minimal_value) {
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

// direct means without rotating; "spds" should be an array of size 3
void move_direct(int16_t *spds)
{
  int compensation = PID(spds, 0);
  motorA.go(spds[0] + compensation);
  motorB.go(spds[1] + compensation);
  motorC.go(spds[2] + compensation);
}

void vpred(int16_t spd) {
  int16_t speeds[3] = {spd, -spd, 0};
  motion_last_dir = 0;
  move_direct(speeds);
}

void vpravo_vpred(int16_t spd) {
  int16_t speeds[3] = {spd, 0, -spd};
  motion_last_dir = 1;
  move_direct(speeds);
}

void vpravo(int16_t spd) {
  int16_t speeds[3] = {spd / 2, spd / 2, -spd};
  motion_last_dir = 2;
  move_direct(speeds);
}

void vpravo_vzad(int16_t spd) {
  int16_t speeds[3] = {0, spd, -spd};
  motion_last_dir = 3;
  move_direct(speeds);
}

void vzad(int16_t spd) {
  int16_t speeds[3] = { -spd, spd, 0};
  motion_last_dir = 4;
  move_direct(speeds);
}

void vlavo_vzad(int16_t spd) {
  int16_t speeds[3] = { -spd, 0, spd};
  motion_last_dir = 5;
  move_direct(speeds);
}

void vlavo(int16_t spd) {
  int16_t speeds[3] = { -spd / 2, -spd / 2, spd};
  motion_last_dir = 6;
  move_direct(speeds);
}

void vlavo_vpred(int16_t spd) {
  int16_t speeds[3] = {0, -spd, spd};
  motion_last_dir = 7;
  move_direct(speeds);
}

void na_mieste() {
  int16_t speeds[3] = {0, 0, 0};
  move_direct(speeds);
}

void motors_off() {
  motorA.go(0);
  motorB.go(0);
  motorC.go(0);
}
