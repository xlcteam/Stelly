Motor motorA = Motor(MOTOR_A_DIR_PIN, MOTOR_A_PWM_PIN);
Motor motorB = Motor(MOTOR_B_DIR_PIN, MOTOR_B_PWM_PIN);
Motor motorC = Motor(MOTOR_D_DIR_PIN, MOTOR_D_PWM_PIN);

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

int16_t spd_to_north(int16_t speeds[3])
{
    int16_t angle = compass_angle();
    if (angle > 180) angle -= 360;

    int16_t max_speed, max_diff, compensation;

    max_speed = abs(speeds[0]);
    if (abs(speeds[1]) > max_speed) { max_speed = abs(speeds[1]); }
    if (abs(speeds[2]) > max_speed) { max_speed = abs(speeds[2]); }

    max_diff = 255 - max_speed;

    if (angle > TO_NORTH_THRESH) {
        compensation = ROTATE_SPEED;
    } else if (angle < -TO_NORTH_THRESH) {
        compensation = -ROTATE_SPEED;
    } else {
        compensation = ROTATE_SPEED * angle / TO_NORTH_THRESH;
        compensation = compensation * abs(angle) / TO_NORTH_THRESH;
    }

    return constrain(compensation, -max_diff, max_diff);
}

void move_raw(int16_t speeds[3])
{
    motorA.go(speeds[0]);
    motorB.go(speeds[1]);
    motorC.go(speeds[2]);
}

void move_to_north(int16_t speeds[3])
{
    int16_t compensation = spd_to_north(speeds);
    motorA.go(speeds[0] + compensation);
    motorB.go(speeds[1] + compensation);
    motorC.go(speeds[2] + compensation);
}

inline void move_up(uint8_t spd)
{
    int16_t speeds[] = { -spd, spd, 0 };
    move_to_north(speeds);
}

inline void move_right(uint8_t spd)
{
    int16_t speeds[] = { -spd/2, -spd/2, spd };
    move_to_north(speeds);
}

inline void move_back(uint8_t spd)
{
    int16_t speeds[] = { spd, -spd, 0 };
    move_to_north(speeds);
}

inline void move_left(uint8_t spd)
{
    int16_t speeds[] = { spd/2, spd/2, -spd };
    move_to_north(speeds);
}

inline void move_up_right(uint8_t spd)
{
    int16_t speeds[] = { -spd, 0, spd };
    move_to_north(speeds);
}

inline void move_back_right(uint8_t spd)
{
    int16_t speeds[] = { 0, -spd, spd };
    move_to_north(speeds);
}

inline void move_back_left(uint8_t spd)
{
    int16_t speeds[] = { spd, 0, -spd };
    move_to_north(speeds);
}

inline void move_up_left(uint8_t spd)
{
    int16_t speeds[] = { 0, spd, -spd };
    move_to_north(speeds);
}

inline void rotate(int16_t spd)
{
    int16_t speeds[] = { spd, spd, spd };
    move_raw(speeds);
}

/* may be dangerous */
inline void centralize()
{
    int16_t spds[] = {0};
    move_to_north(spds);
}

/* VIC FUNCTIONS */

void vic_move_to_north(void)
{
    int16_t spds[3];

    if (vic_args("%d %d %d", &spds[0], &spds[1], &spds[2]) == 3) {
        move_to_north(spds);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
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
