uint8_t line_sensors[LINE_SENSORS_COUNT] =
    { LINE_S1_PIN, LINE_S2_PIN, LINE_S4_PIN };

void setup_line_sensors()
{
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        pinMode(line_sensors[i], INPUT);
    }
    pinMode(LINE_THRESH_PIN, OUTPUT);

    PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);

    line_sensors_update();
}

inline uint8_t read_line_sensor(uint8_t index)
{
    return digitalRead(line_sensors[index]);
}

void line_sensors_update()
{
    analogWrite(LINE_THRESH_PIN, light_pwm);
    PCICR &= ~_BV(PCIE0); // clear
    PCICR |= (line_use_int & 0x01) << PCIE0; // set
    led_set(2, line_use_int);
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

    mutex[0] = 1;
    mutex[1] = 1;
    mutex[2] = 1;

    if (!ws[0] && !ws[1] && !ws[2]) {
        dir = 255;
    } else if (ws[0] && !ws[1] && !ws[2]) {
        if (motion_last_dir < 4 && motion_last_dir > 0) {
            dir = 5;
        } else if (motion_last_dir > 4 && motion_last_dir <= 7) {
            dir = 3;
        } else {
            dir = 4;
        }
    } else if (ws[0] && ws[1] && !ws[2]) {
        if (abs((int32_t)ws[0] - (int32_t)ws[1]) < LINE_MAX_DIFF_TIME) {
            dir = 5;
        } else if (ws[0] < ws[1]) {
            dir = 5;
        } else { /* ws[1] < ws[0] */
            dir = 6;
        }
    } else if (ws[0] && !ws[1] && ws[2]) {
        if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 3;
        } else if (ws[0] < ws[2]) {
            dir = 3;
        } else { /* ws[2] < ws[0] */
            dir = 2;
        }
    } else if (!ws[0] && ws[1] && ws[2]) {
        dir = 0;
    } else if (!ws[0] && !ws[1] && ws[2]) {
        if (motion_last_dir > 4 || motion_last_dir == 0) {
            dir = 2;
        } else {
            dir = 1;
        }
    } else if (!ws[0] && ws[1] && !ws[2]) {
        if (motion_last_dir >= 0 && motion_last_dir < 4) {
            dir = 6;
        } else {
            dir = 7;
        }
    } else { /* (ws[0] && ws[1] && ws[2]) */

        if (abs((int32_t)ws[0] - (int32_t)ws[1]) < LINE_MAX_DIFF_TIME
        &&  abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME
        &&  abs((int32_t)ws[1] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = line_last_dir;
        } else if (abs((int32_t)ws[0] - (int32_t)ws[1]) < LINE_MAX_DIFF_TIME) {
            if (ws[2] < ws[0]) {
                dir = 1;
            } else {
                dir = 5;
            }
        } else if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            if (ws[1] < ws[0]) {
                dir = 7;
            } else {
                dir = 3;
            }
        } else if (abs((int32_t)ws[1] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            if (ws[0] < ws[1]) {
                dir = 4;
            } else {
                dir = 0;
            }
        } else if (ws[1] < ws[0] && ws[0] < ws[2]) {
            dir = 6;
        } else if (ws[2] < ws[0] && ws[0] < ws[1]) {
            dir = 2;
        } else if (ws[1] < ws[2]) {
            dir = 7;
        } else if (ws[2] < ws[1]) {
            dir = 1;
        } else if (ws[0] < ws[1] && ws[1] < ws[2]) {
            dir = 5;
        } else { /* ws[0] < ws[2] && ws[2] < ws[1] */
            dir = 3;
        }
    }

    mutex[0] = 0;
    mutex[1] = 0;
    mutex[2] = 0;

    line_last_dir = dir;

    return dir;
}

uint8_t line_sensors_dir()
{
    Serial.println("line sensors dir");
    Serial.print(ws[0]);
    Serial.print(" "); Serial.print(ws[1]);
    Serial.print(" "); Serial.println(ws[2]);

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
        mutex[0] = mutex[1] = mutex[2] = 1;
        uint8_t count = (bool)ws[0] + (bool)ws[1] + (bool)ws[2];
        mutex[0] = mutex[1] = mutex[2] = 0;
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
        mutex[0] = mutex[1] = mutex[2] = 1;
        uint8_t count = (bool)ws[0] + (bool)ws[1] + (bool)ws[2];
        mutex[0] = mutex[1] = mutex[2] = 0;
        if (count == 1) {
            if (millis() - s_time > min_time) {
                Serial.print("no line");
                state = 0;
                dir = 255;
                mutex[0] = mutex[1] = mutex[2] = 1;
                ws[0] = ws[1] = ws[2] = 0;
                mutex[0] = mutex[1] = mutex[2] = 0;
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
        mutex[0] = mutex[1] = mutex[2] = 1;
        ws[0] = read_line_sensor(0) * micros();
        ws[1] = read_line_sensor(1) * micros();
        ws[2] = read_line_sensor(2) * micros();
        mutex[0] = mutex[1] = mutex[2] = 0;
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
