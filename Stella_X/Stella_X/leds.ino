uint8_t leds[LEDS_COUNT] = { LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN };

void setup_leds()
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++) {
        pinMode(leds[i], OUTPUT);
    }
}

inline void led_set(uint8_t index, uint8_t state)
{
    digitalWrite(leds[index], state);
}

void leds_set_all(uint8_t states)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++) {
        digitalWrite(leds[i], states & (0x01 << i));
    }
}

/* VIC FUNCTIONS */

void vic_led_set(void)
{
    uint8_t index, state;
    if (vic_args("%hhu %hhu", &index, &state) == 2) {
        led_set(index, state);
    }
}

void vic_leds_set_all(void)
{
    uint8_t states;
    if (vic_args("%hhu", &states) == 1) {
        leds_set_all(states);
    }
}
