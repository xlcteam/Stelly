uint8_t line_sensors[LINE_SENSORS_COUNT] =
    { LINE_FRONT_PIN, LINE_RIGHT_PIN, LINE_LEFT_PIN };
/* all interrupt pins are on the same port, so we can use any one of them
   to determine PCMSK, PCICR and PCIE
*/

bool line_sensors_vic_dbg = false;

void setup_line_sensors()
{
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        uint8_t pin = line_sensors[i];
        pinMode(pin, INPUT);
        *digitalPinToPCMSK(LINE_FRONT_PIN) |= _BV(digitalPinToPCMSKbit(pin));
    }
    pinMode(LINE_THRESH_PIN, OUTPUT);

    line_sensors_update();
}

inline uint8_t read_line_sensor(uint8_t index)
{
    return digitalRead(line_sensors[index]);
}

void line_sensors_update()
{
    analogWrite(LINE_THRESH_PIN, light_pwm);

    *digitalPinToPCICR(LINE_FRONT_PIN) &=
        ~_BV(digitalPinToPCICRbit(LINE_FRONT_PIN)); // clear
    if (line_use_int) {
        *digitalPinToPCICR(LINE_FRONT_PIN) |=
            _BV(digitalPinToPCICRbit(LINE_FRONT_PIN)); // set
    }
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

    MUTEX(1);

    if (!ws[0] && !ws[1] && !ws[2]) {
        line_level = 0;
        dir = 255;
    } else if (ws[0] && !ws[1] && !ws[2]) {
        line_level = 1;
        dir = 4; stop_next_to_line = 0;
        /*
        if (motion_last_dir < 4 && motion_last_dir > 0) {
            dir = 5;
        } else if (motion_last_dir > 4 && motion_last_dir <= 7) {
            dir = 3;
        } else {
            dir = 4;
        }
        */
    } else if (ws[0] && ws[1] && !ws[2]) {
        line_level = 1;
        if (abs((int32_t)ws[0] - (int32_t)ws[1]) < LINE_MAX_DIFF_TIME) {
            dir = 5; stop_next_to_line = 1;
        } else if (ws[0] < ws[1]) {
            dir = 5; stop_next_to_line = 1;
        } else { /* ws[1] < ws[0] */
            line_level = 2;
            dir = 5; stop_next_to_line = 2;
        }
    } else if (ws[0] && !ws[1] && ws[2]) {
        line_level = 1;
        if (abs((int32_t)ws[0] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 3; stop_next_to_line = 7;
        } else if (ws[0] < ws[2]) {
            dir = 3; stop_next_to_line = 7;
        } else { /* ws[2] < ws[0] */
            line_level = 2;
            dir = 3; stop_next_to_line = 6;
        }
    } else if (!ws[0] && ws[1] && ws[2]) {
        line_level = 2;
        if (abs((int32_t)ws[1] - (int32_t)ws[2]) < LINE_MAX_DIFF_TIME) {
            dir = 0; stop_next_to_line = 4;
        } else if (ws[1] < ws[2]) {
            dir = 0; stop_next_to_line = 4;
        } else { /* ws[2] < ws[1] */
            dir = 0; stop_next_to_line = 4;
        }
    } else if (!ws[0] && !ws[1] && ws[2]) {
        line_level = 1;
        stop_next_to_line = 6;
        if (motion_last_dir > 4 || motion_last_dir == 0) {
            dir = 2;
        } else {
            dir = 1;
        }
    } else if (!ws[0] && ws[1] && !ws[2]) {
        line_level = 1;
        stop_next_to_line = 2;
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
        stop_next_to_line = (dir + 4) % 8;
    }

    MUTEX(0);

    line_last_dir = dir;

    return dir;
}

uint8_t line_sensors_dir()
{
	uint32_t time = micros();

    // get data to process
    if (!line_use_int) {
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            ws_tmp[i] = read_line_sensor(i) * time;
        }
    }

    MUTEX(1);
	for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
		if (ws[i] == 0) {
			ws[i] = ws_tmp[i];
		} else if (ws_tmp[i] != 0) {
            ws[i] = ws_tmp[i];
            //Serial.println("b");
            /*
        } else if (ws_tmp[(i+1)%3] || ws_tmp[(i+2)%3]) {
            Serial.println(time);
            if (time - ws[i] > 500000) {
                ws[i] = 0;
                Serial.println("c");
            }
            */
            /* TODO: bellow constant */
        } else if (time - ws[i] > 50000 + (line_level - 1) * LINE_EXTRA_TIME * 1000) {
            //Serial.println(time);
            ws[i] = 0;
            //Serial.println("d");
        }
	}
    MUTEX(0);

    /*
    if (time - ws_last[4] < 500000 && (ws[0] || ws[1] || ws[2])) {
        Serial.print("ws_last: ");
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            if (ws[i] == 0) {
                ws[i] = ws_last[i];
                Serial.print(i);
            }
        }
        Serial.println();
    }
    */

    bool print_dbg = line_sensors_vic_dbg && (ws[0] || ws[1] || ws[2]);

    if (print_dbg) {
        Serial.println("line");
        Serial.print(ws[0]);
        Serial.print(" "); Serial.print(ws[1]);
        Serial.print(" "); Serial.println(ws[2]);
    }

    uint16_t dir = process_ws();

    // fill ws_tmp with actual values
    if (line_use_int) {
        MUTEX(1);
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            ws_tmp[i] = read_line_sensor(i) * micros();
        }
        MUTEX(0);
    }

    if (print_dbg) {
        Serial.print("dir "); Serial.println(dir);
    }

    /*
	if (dir != 255 && ws_last[3] != dir) {
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
            ws_last[i] = ws[i];
        }
		ws_last[3] = dir;
		ws_last[4] = time;
        Serial.println("change 2");
	} else if (dir != 255 && ws_last[3] == dir) {
        ws_last[4] = time;
    }
    */

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

void vic_debug_line(void)
{
    uint8_t dbg = 1;
    vic_args("%hhu", &dbg);
    line_sensors_vic_dbg = dbg;
}
