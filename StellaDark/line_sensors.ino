/**
 * functions for line sensors procesing
 */
uint8_t line_sensors[LINE_SENSORS_COUNT] =
{ LINE_FRONT_PIN, LINE_RIGHT_PIN, LINE_LEFT_PIN };
/* all interrupt pins are on the same port, so we can use any one of them
	 to determine PCMSK, PCICR and PCIE
*/

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
		dir = 255;
	} else if (ws[0] && !ws[1] && !ws[2]) {
		dir = 0;
	} else if (!ws[0] && ws[1] && !ws[2]) {
		if (motion_last_dir == 3) {
			dir = 3;
		} else if (motion_last_dir <= 5 && motion_last_dir >= 4) {
			dir = 4;
		} else {
			dir = 2;
		}
	} else if (!ws[0] && !ws[1] && ws[2]) {
		if (motion_last_dir == 5) {
			dir = 5;
		} else if (motion_last_dir <= 4 && motion_last_dir >= 3) {
			dir = 4;
		} else {
			dir = 6;
		}
	} else if (ws[0] && ws[1] && !ws[2]) {
		dir = 1;
	} else if (ws[0] && !ws[1] && ws[2]) {
		dir = 7;
	} else if (!ws[0] && ws[1] && ws[2]) {
		if (motion_last_dir <= 6 && motion_last_dir >= 5) {
			dir = 5;
		} else if (motion_last_dir <= 3 && motion_last_dir >= 2) {
			dir = 3;
		} else {
			dir = 4;
		}
	} else {
		dir = line_last_dir;
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
			ws[i] = read_line_sensor(i);
		}
	}

	bool print_dbg = (ws[0] || ws[1] || ws[2]);

	if (print_dbg) {
		Serial.println("line"); Serial.print(ws[0]);
		Serial.print(" "); Serial.print(ws[1]);
		Serial.print(" "); Serial.println(ws[2]);
	}

	uint16_t dir = process_ws();

	// fill ws_tmp with actual values
	if (line_use_int) {
		MUTEX(1);
		for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
			ws[i] = read_line_sensor(i);
		}
		MUTEX(0);
	}

	if (print_dbg) {
		Serial.print("dir "); Serial.println(dir);
	}

	return dir;
}
