Motor motorA = Motor(MOTOR_A_DIR_PIN, MOTOR_A_PWM_PIN);
Motor motorB = Motor(MOTOR_B_DIR_PIN, MOTOR_B_PWM_PIN);
Motor motorC = Motor(MOTOR_C_DIR_PIN, MOTOR_C_PWM_PIN);
Motor motorD = Motor(MOTOR_D_DIR_PIN, MOTOR_D_PWM_PIN);

void brake(void)
{
    motorA.off();
    motorB.off();
    motorC.off();
    motorD.off();
}

void halt(void)
{
    motorA.stop();
    motorB.stop();
    motorC.stop();
    motorD.stop();
}

void move_raw(int16_t speeds[4])
{
    motorA.go(speeds[0]);
    motorB.go(speeds[1]);
    motorC.go(speeds[2]);
    motorD.go(speeds[3]);
}

void move_PID(int16_t speeds[4])
{
    int16_t compensation = 0;
    motorA.go(speeds[0] + compensation);
    motorB.go(speeds[1] + compensation);
    motorC.go(speeds[2] + compensation);
    motorD.go(speeds[3] + compensation);
}

inline void move_up(uint8_t spd)
{
    int16_t speeds[] = { spd, -spd, -spd, spd };
    move_PID(speeds);
}

inline void move_right(uint8_t spd)
{
    int16_t speeds[] = { spd, spd, -spd, -spd };
    move_PID(speeds);
}

inline void move_back(uint8_t spd)
{
    int16_t speeds[] = { -spd, spd, spd, -spd };
    move_PID(speeds);
}

inline void move_left(uint8_t spd)
{
    int16_t speeds[] = { -spd, -spd, spd, spd };
    move_PID(speeds);
}

inline void move_up_right(uint8_t spd)
{
    int16_t speeds[] = { spd, 0, -spd, 0 };
    move_PID(speeds);
}

inline void move_back_right(uint8_t spd)
{
    int16_t speeds[] = { 0, spd, 0, -spd };
    move_PID(speeds);
}

inline void move_back_left(uint8_t spd)
{
    int16_t speeds[] = { -spd, 0, spd, 0 };
    move_PID(speeds);
}

inline void move_up_left(uint8_t spd)
{
    int16_t speeds[] = { 0, -spd, 0, spd};
    move_PID(speeds);
}

inline void rotate(int16_t spd)
{
    int16_t speeds[] = { spd, spd, spd, spd };
    move_raw(speeds);
}

/* may be dangerous */
inline void centralize()
{
    int16_t spds[] = {0};
    move_PID(spds);
}

