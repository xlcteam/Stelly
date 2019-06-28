void (*movements[])(int16_t) = { forward, right_forward, right, right_backward,
	backward, left_backward, left, left_forward };

void (*movements_by_front_IR[])(int16_t) = {
	backward, left_backward, left_backward,
	left, forward, right,
	right_backward, right_backward, backward };

void (*movements_by_back_IR_near[])(int16_t) = {
	right_backward, right_backward, backward,
	right, right, right,
	backward, left_backward, left_backward };

void (*movements_by_back_IR_far[])(int16_t) = {
	right_backward, right_backward, backward,
	backward, backward, backward,
	backward, left_backward, left_backward };

void from_line(uint8_t line_dir) {
	uint32_t delay_time = LINE_TIME;
	if (line_dir >= 3 && line_dir <= 5) {
		delay_time = LINE_BACK_TIME;
	}
	void (*move_fn)(int16_t) = movements[(line_dir + 4) % 8];

	start = millis();
	while (millis() - start < LINE_STRONG_TIME) {
		move_fn(LINE_STRONG_SPD);
	}
	while (millis() - start < delay_time) {
		move_fn(LINE_SPD);
	}
	start = millis();
	do {
		motors_off();
	} while (millis() - start < STOP_LINE_TIME);
}

void move_by_front_IR(uint8_t dir, int16_t spd) {
	if (dir == 0) {
		stay_direct();
	} else {
		movements_by_front_IR[dir - 1](spd);
	}
}

void move_by_back_IR(uint8_t dir, int16_t spd, bool near) {
	if (dir == 0) {
		stay_direct();
	} else if (near) {
		movements_by_back_IR_near[dir - 1](spd);
	} else {
		movements_by_back_IR_far[dir - 1](spd);
	}
}
