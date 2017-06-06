int8_t dribbler_state;

void setup_dribbler()
{
    pinMode(DRIBBLER_PIN, OUTPUT);
    pinMode(DRIBBLER_SENSOR_PIN, INPUT);
    dribbler_off();
}

inline void dribbler_off()
{
    digitalWrite(DRIBBLER_PIN,0);
    dribbler_state = 0;
}

inline void dribbler_on()
{
    digitalWrite(DRIBBLER_PIN, 1);
    dribbler_state = 1;
}

inline void dribbler_switch()
{
    digitalWrite(DRIBBLER_PIN, dribbler_state = !dribbler_state);
}

bool ball_in_dribbler()
{
    static uint32_t l_time = millis();
    if (!digitalRead(DRIBBLER_SENSOR_PIN)) {
        l_time = millis();
        return true;
    }
    if (millis() - l_time < DRIBBLER_SENSOR_TIME) {
        return true;
    }
    return false;
}

/* VIC FUNCTIONS */

void vic_dribbler_off(void)
{
    dribbler_off();
}

void vic_dribbler_on(void)
{
    dribbler_on();
}

void vic_ball_in_dribbler(void)
{
    vic_printf("%u\n", (uint8_t) ball_in_dribbler());
}
