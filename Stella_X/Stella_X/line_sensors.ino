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
        line_level = 0;
        dir = 255;
    } else if (ws[0] && !ws[1] && !ws[2]) {
        line_level = 1;
        if (motion_last_dir < 4 && motion_last_dir > 0) {
            //if (line_last_dir
            dir = 5;
        } else if (motion_last_dir > 4 && motion_last_dir <= 7) {
            dir = 3;
        } else {
            dir = 4;
        }
    } else if (ws[0] && ws[1] && !ws[2]) {
        line_level = 1;
        if (abs((int32_t)ws[0] - (int32_t)ws[1]) < LINE_MAX_DIFF_TIME) {
            dir = 5;
        } else if (ws[0] < ws[1]) {
            dir = 5;
        } else { /* ws[1] < ws[0] */
            line_level = 2;
            dir = 6;
        }
    } else if (ws[0] && !ws[1] && ws[2]) {
        line_level = 1;
        if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 3;
        } else if (ws[0] < ws[2]) {
            dir = 3;
        } else { /* ws[2] < ws[0] */
            line_level = 2;
            dir = 2;
        }
    } else if (!ws[0] && ws[1] && ws[2]) {
        line_level = 1;
        dir = 0;
    } else if (!ws[0] && !ws[1] && ws[2]) {
        line_level = 1;
        if (motion_last_dir > 4 || motion_last_dir == 0) {
            dir = 2;
        } else {
            dir = 1;
        }
    } else if (!ws[0] && ws[1] && !ws[2]) {
        line_level = 1;
        if (motion_last_dir >= 0 && motion_last_dir < 4) {
            dir = 6;
        } else {
            dir = 7;
        }
    } else { /* (ws[0] && ws[1] && ws[2]) */
        line_level = 3;
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

    // get data to process
    if (!line_use_int) {
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            ws[i] = read_line_sensor(i) * micros();
        }
    }

    uint16_t dir = process_ws();

    // fill ws with actual values
    if (line_use_int) {
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            mutex[i] = 1;
            ws[i] = read_line_sensor(i) * micros();
            mutex[i] = 0;
        }
    }

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
