Motor motorA = Motor(MOTOR_A_DIR_PIN, MOTOR_A_PWM_PIN);
Motor motorB = Motor(MOTOR_B_DIR_PIN, MOTOR_B_PWM_PIN);
Motor motorC = Motor(MOTOR_C_DIR_PIN, MOTOR_C_PWM_PIN);

void brake(void)
{
    motorA.off();
    motorB.off();
    motorC.off();
}

void halt(void)
{
    motorA.stop();
    motorB.stop();
    motorC.stop();
}

int32_t P_term = 0;
int32_t I_term = 0;
int32_t D_term = 0;
uint32_t PID_last_time = 0;
bool PID_reset = true;

int16_t PID(int16_t speeds[3])
{
    int32_t min_limit = -1000;
    int32_t max_limit = 1000;
    for (uint8_t i = 0; i < 3; i++) {
        if (255 - speeds[i] < max_limit) {
            max_limit = 255 - speeds[i];
        }
        if (-255 - speeds[i] > min_limit) {
            min_limit = -255 - speeds[i];
        }
    }

    int32_t error = (int32_t)compass_angle();
    if (error >= 180) error -= 360;

    if (PID_reset) {
        int32_t output = error * kP / 100;
        if (output > max_limit) {
            output = max_limit;
        } else if (output < min_limit) {
            output = min_limit;
        }
        PID_last_time = micros();

        return output;
    }

    int32_t d_time = micros() - PID_last_time;

    P_term = error * kP; // [error]%
    int32_t output = (P_term + D_term) / 100;

    // if P_term + D_term exceeds limits, don't use I_term
    if (output > max_limit) {
        output = max_limit;
        I_term = 0;
    } else if (output < min_limit) {
        output = min_limit;
        I_term = 0;
    } else {
        I_term += error * d_time / 1000 * kI / 1000; // [error]% * mS
        output += I_term / 100;
        // solve reset-windup
        if (output > max_limit) {
            I_term -= (output - max_limit) * 100;
            output = max_limit;
        } else if (output < min_limit) {
            I_term += (min_limit - output) * 100;
            output = min_limit;
        }
    }

    PID_last_time = micros();

    return output;
}

void restart_PID()
{
    P_term = I_term = D_term = 0;
    PID_reset = true;
}

void move_raw(int16_t speeds[3])
{
    motorA.go(speeds[0]);
    motorB.go(speeds[1]);
    motorC.go(speeds[2]);
}

void move_PID(int16_t speeds[3])
{
    int16_t compensation = PID(speeds);
    motorA.go(speeds[0] - compensation);
    motorB.go(speeds[1] - compensation);
    motorC.go(speeds[2] - compensation);
}

inline void move_up(uint8_t spd)
{
    int16_t speeds[] = { spd, -spd, 0 };
    move_PID(speeds);
}

inline void move_right(uint8_t spd)
{
    int16_t speeds[] = { spd/2, spd/2, -spd };
    move_PID(speeds);
}

inline void move_back(uint8_t spd)
{
    int16_t speeds[] = { -spd, spd, 0 };
    move_PID(speeds);
}

inline void move_left(uint8_t spd)
{
    int16_t speeds[] = { -spd/2, -spd/2, spd };
    move_PID(speeds);
}

inline void move_up_right(uint8_t spd)
{
    int16_t speeds[] = { spd, 0, -spd };
    move_PID(speeds);
}

inline void move_back_right(uint8_t spd)
{
    int16_t speeds[] = { 0, spd, -spd };
    move_PID(speeds);
}

inline void move_back_left(uint8_t spd)
{
    int16_t speeds[] = { -spd, 0, spd };
    move_PID(speeds);
}

inline void move_up_left(uint8_t spd)
{
    int16_t speeds[] = { 0, -spd, spd };
    move_PID(speeds);
}

inline void rotate(int16_t spd)
{
    int16_t speeds[] = { -spd, -spd, -spd };
    move_raw(speeds);
}

/* may be dangerous */
inline void centralize()
{
    int16_t spds[] = {0, 0, 0};
    move_PID(spds);
}

/*void cetrilizing() 
{
  while(compass_angle < )
  }*/

/* VIC FUNCTIONS */

void vic_move_PID(void)
{
    int16_t spds[3];

    if (vic_args("%d %d %d", &spds[0], &spds[1], &spds[2]) == 3) {
        move_PID(spds);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_restart_PID(void)
{
    restart_PID();
}

void vic_mA(void) { int16_t s; vic_args("%d", &s); motorA.go(s); }
void vic_mB(void) { int16_t s; vic_args("%d", &s); motorB.go(s); }
void vic_mC(void) { int16_t s; vic_args("%d", &s); motorC.go(s); }

void vic_move_up(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_up_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_up_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}
