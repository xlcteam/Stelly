uint8_t line_sensors[LINE_SENSORS_COUNT] = { LINE_S_FRONT_PIN,
	LINE_S_RIGHT1_PIN, LINE_S_RIGHT2_PIN, LINE_S_BACK_PIN, LINE_S_LEFT1_PIN,
	LINE_S_LEFT2_PIN };
// PB7, PB5, PJ1, PB4, PB6, PJ0

void setup_line_sensors()
{
	for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
		pinMode(line_sensors[i], INPUT);
	}
	pinMode(LINE_S_THRESH_PIN, OUTPUT);
    PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);
    PCMSK1 |= _BV(PCINT9) | _BV(PCINT10);
    line_sensors_update();
}

inline uint8_t read_line_sensor(uint8_t index)
{
    return digitalRead(line_sensors[index]);
}

ISR(PCINT0_vect)
{
    if (mutex) return;
    if (PINB & _BV(PB4) && !ws[3]) {
        ws[3] = micros();
        halt();
    }
    if (PINB & _BV(PB5) && !ws[1]) {
        ws[1] = micros();
        halt();
    }
    if (PINB & _BV(PB6) && !ws[4]) {
        ws[4] = micros();
        halt();
    }
    if (PINB & _BV(PB7) && !ws[0]) {
        ws[0] = micros();
        halt();
    }
}

ISR(PCINT1_vect)
{
    if (mutex) return;
    if (PINJ & _BV(PJ0) && !ws[5]) {
        ws[5] = micros();
        halt();
    }
    if (PINJ & _BV(PJ1) && !ws[2]) {
        ws[2] = micros();
        halt();
    }
}


void line_interrupt_on()
{
    PCICR |= _BV(PCIE0) | _BV(PCIE1);
    last_line_use_int = line_use_int = 1;
}

void line_interrupt_off()
{
    PCICR &= ~(_BV(PCIE0) | _BV(PCIE1));
    last_line_use_int = line_use_int = 0;
}

void line_sensors_update()
{
    if (last_line_use_int && !line_use_int) {
        line_interrupt_off();
    } else if (!last_line_use_int && line_use_int) {
        line_interrupt_on();
    }
    last_line_use_int = line_use_int;
    analogWrite(LINE_S_THRESH_PIN, light_pwm);
    led_set(2, line_use_int);
}

uint32_t predict_ws0()
{
    if ((ws[1] || ws[2]) && (ws[4] || ws[5]) && !ws[3]) {
        uint32_t min_time = ~(uint32_t)1;
        for (uint8_t i = 1; i < 6; i++) {
            if (ws[i] != 0 && ws[i] < min_time) {
                min_time = ws[i];
            }
        }
        ws[0] = min_time - 2 * LINE_MAX_DIFF_TIME;
    } else if ((ws[1] || ws[2]) && (ws[4] || ws[5]) && ws[3]) {
        uint32_t min_time_i = 1;
        for (uint8_t i = 1; i < 6; i++) {
            if (ws[i] != 0 && ws[i] < ws[min_time_i]) {
                min_time_i = i;
            }
        }
        if (min_time_i != 3) {
            ws[0] = ws[min_time_i] - 2 * LINE_MAX_DIFF_TIME;
        }
    } else {
        ws[0] = 0;
    }
}

/* decide where to move after reaching the white line (processes ws)
   dirs:
   0 - 7: multiplied by 45 gives angle where to move
   255: no line
*/
uint8_t line_last_dir;
uint8_t process_ws()
{
    uint8_t dir;

    mutex = 1;

    if (!ws[0] && !ws[1] && !ws[2] && !ws[3] && !ws[4] && !ws[5]) {
        dir = 255;
    } else if (ws[0] && !ws[1] && !ws[2] && !ws[3] && !ws[4] && !ws[5]) {
        if (motion_last_dir < 180 && motion_last_dir > 15) {
            dir = 5;
        } else if (motion_last_dir > 180 && motion_last_dir < 345) {
            dir = 3;
        } else {
            dir = 4;
        }
    } else if (!ws[0] && !ws[1] && !ws[2] && ws[3] && !ws[4] && !ws[5]) {
        if (motion_last_dir < 165 && motion_last_dir > 0) {
            dir = 7;
        } else if (motion_last_dir > 195 && motion_last_dir < 360) {
            dir = 1;
        } else {
            dir = 0;
        }
    } else if (ws[0] && ((ws[1] || ws[2]) == (ws[4] || ws[5])) && !ws[3]) {
        dir = 4;
    } else if (!ws[0] && ((ws[1] || ws[2]) == (ws[4] || ws[5])) && ws[3]) {
        dir = 0;
    } else if (ws[0] && (ws[1] || ws[2]) && !ws[3] && !ws[4] && !ws[5]) {
        dir = 5;
        /*
        if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 5;
        } else if (!ws[2]) {
            dir = 5;
        } else if (ws[0] < ws[2]) {
            dir = 5;
        } else { // ws[1] < ws[0]
            dir = 6;
        }
        */
    } else if (ws[0] && !ws[1] && !ws[2] && !ws[3] && (ws[4] || ws[5])) {
        dir = 3;
        /*
        if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 3;
        } else if (ws[0] < ws[2]) {
            dir = 3;
        } else { // ws[2] < ws[0]
            dir = 2;
        }
        */
    } else if (!ws[0] && (ws[1] || ws[2]) && ws[3] && !ws[4] && !ws[5]) {
        dir = 7;
    } else if (!ws[0] && !ws[1] && !ws[2] && ws[3] && (ws[4] || ws[5])) {
        dir = 1;
    } else if ((ws[1] || ws[2]) && (ws[0] == ws[3]) && !ws[4] && !ws[5]) {
        dir = 6;
    } else if (!ws[1] && !ws[2] && (ws[0] == ws[3]) && (ws[4] || ws[5])) {
        dir = 2;
    } else if (ws[0] && (ws[1] || ws[2]) && ws[3] && (ws[4] || ws[5])) {
        uint8_t latest_i = 0;
        for (uint8_t i = 0; i < 6; i++) {
            if (ws[i] > ws[latest_i]) {
                latest_i = i;
            }
        }
        if (latest_i == 0) {
            dir = 0;
        } else if (latest_i == 3) {
            dir = 4;
        } else if (latest_i == 1 || latest_i == 2) {
            dir = 2;
        } else {
            dir = 6;
        }
    } else { // very bad
        dir = (line_last_dir / 45 + 4) % 4; // opposite direction
    }

    mutex = 0;

    line_last_dir = dir;

    return dir;
}

uint8_t line_sensors_dir()
{
    /*
    Serial.println("line sensors dir");
    Serial.print(ws[0]);
    Serial.print(" "); Serial.print(ws[1]);
    Serial.print(" "); Serial.println(ws[2]);
    */

    static uint8_t dir = 255;
    static uint32_t d_time = 0;
    static uint32_t s_time = 0;
    static uint32_t min_time = 0;
    static uint8_t state = 0;

#if 0
    if (!line_use_int) {
        /* load actual values */
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            ws[i]  = read_line_sensor(i) * micros();
        }
    }
#endif
    line_speed_down = 0;

    if (state == 0) {
        Serial.print("0 ");
        mutex = 1;
        uint8_t count = (bool)ws[0] + (bool)ws[1] + (bool)ws[2];
        mutex = 0;
        if (count == 0) {
            state = 0;
            dir = 255;
        } else if (count == 1) {
            if (ws[0] == 0) {
                Serial.print("speed down");
                state = 1;
                min_time = LINE_TIME_MIN;
                dir = motion_last_dir;
                line_speed_down = 1;
            } else {
                Serial.print("ws0");
                state = 2;
                min_time = LINE_TIME_MIN / 2;
                dir = 4;
            }
            d_time = s_time = millis();
        } else {
            state = 2;
            min_time = (count == 2) ? LINE_TIME_MIN : LINE_TIME_MIN * 2;
            d_time = s_time = millis();
            dir = process_ws();
            Serial.print(dir);
            Serial.print(" "); Serial.print(count);
        }

    } else if (state == 1) {
        Serial.print("1 ");
        mutex = 1;
        uint8_t count = (bool)ws[0] + (bool)ws[1] + (bool)ws[2];
        mutex = 0;
        if (count == 1) {
            if (millis() - s_time > min_time) {
                Serial.print("no line");
                state = 0;
                dir = 255;
                mutex = 1;
                ws[0] = ws[1] = ws[2] = 0;
                mutex = 0;
            } else {
                Serial.print("speed down");
                line_speed_down = 1;
            }
        } else {
            state = 2;
            d_time = s_time = millis();
            dir = process_ws();
            Serial.print(dir);
        }

    } else if (state == 2) {
        Serial.print("2 ");
        uint8_t dir_a = process_ws();
        if (dir_a != dir) {
            if (millis() - d_time > LINE_D_TIME_MIN
            &&  millis() - s_time > min_time) {
                state = dir_a == 255 ? 0 : 2; // mozno rozlisit 1 a 2
                d_time = s_time = millis();
                dir = dir_a;
                Serial.print(dir);
            }
        } else {
            Serial.print("wait");
            d_time = millis();
        }
        mutex = 1;
        ws[0] = read_line_sensor(0) * micros();
        ws[1] = read_line_sensor(1) * micros();
        ws[2] = read_line_sensor(2) * micros();
        mutex = 0;
    }

    Serial.print("return "); Serial.println(dir);

    return dir;
}

uint8_t line_for_display()
{
    uint8_t out = 0;
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        out |= read_line_sensor(i) << i;
    }
    return out;
}

/* VIC FUNCTIONS */

void vic_print_line_sensors(void)
{
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        vic_printf("%u ", read_line_sensor(i));
    }
    vic_out('\n');
}

void vic_print_line_sensors_ws(void)
{
    mutex = 1;
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        vic_printf("%lu ", ws[i]);
    }
    vic_out('\n');
    mutex = 0;
}

void vic_print_line_sensors_nice(void)
{
    vic_printf("  %u\n", read_line_sensor(0));
    vic_printf("%u%u %u%u\n", read_line_sensor(5), read_line_sensor(4),
                              read_line_sensor(1), read_line_sensor(2));
    vic_printf("  %u\n", read_line_sensor(3));
}

void vic_print_line_sensors_ws_nice(void)
{
    mutex = 1;
    vic_printf("  %lu\n", ws[0]);
    vic_printf("%lu %lu %lu %lu\n", ws[5], ws[4], ws[1], ws[2]);
    vic_printf("  %lu\n", ws[3]);
    mutex = 0;
}

void vic_zero_ws(void)
{
    mutex = 1;
    ws[0] = ws[1] = ws[2] = ws[3] = ws[4] = ws[5] = 0;
    mutex = 0;
}

void vic_calib_line_sensors(void)
{
    line_interrupt_off();
    uint16_t thresh = light_pwm, thresh_min, thresh_max;
    for (thresh_max = 0; thresh_max < 256; thresh_max++) {
        light_pwm = thresh_max;
        line_sensors_update();
        if (read_line_sensor(0) == 0) {
            delay(50);
            if (read_line_sensor(0) == 0) {
                break;
            }
        }
        delay(20);
    }
    for (thresh_min = 255; thresh_min >= 0; thresh_min--) {
        light_pwm = thresh_min;
        line_sensors_update();
        if (read_line_sensor(0) == 1) {
            delay(50);
            if (read_line_sensor(0) == 1) {
                break;
            }
        }
        delay(20);
    }
    vic_printf("min: %u max: %u suggest: %u",
        thresh_min, thresh_max, (thresh_min + thresh_max) / 2);
    light_pwm = thresh;
}
