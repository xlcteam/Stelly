const uint8_t last_count = 30;
uint16_t last_dirs[last_count];
uint16_t start_north;
uint8_t motion_print;
/* when we can't decide for sure */
char goal_dir = 'L';
char ball_side = 'L';
inline char motion_get_ball_side() { return ball_side; }

void motion_start()
{
    for (uint8_t i = 0; i < last_count; i++) {
        last_dirs[i] = ~0;
    }
    line_sensors_update();
    motion_last_dir = stop_next_to_line = 255;
    stop_next_to_line_time = 0;
    start_north = compass_north();
    restart_PID();
    if (line_use_int) {
        MUTEX(1);
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++){
            ws[i] = 0;
            ws_tmp[i] = 0;
        }
        MUTEX(0);
    }
    lcd.clear();
    lcd.print("motion");
    led_set(0, 1);
}

void motion()
{
    static uint32_t time = millis();
    motion_print = motion_dbg && millis() - time > LCD_DELAY;
    if (motion_print) {
        lcd.clear();
        lcd.print("motion C");
        lcd.print(compass_angle());
        lcd.print(" T");
        lcd.print(delta_time);
        time = millis();
    }
    uint16_t dir = line_sensors_dir();

    if (dir != 255) {
        motion_line(dir);
        ball_side = ball_side == 'L' ? 'R' : 'L';
        goal_dir = goal_dir == 'L' ? 'R' : 'L';
    } else {
        if (use_pixy == 2) {
            pixy_motion_ball();
        } else {
            uint16_t dir_raw = dir_to_ball();

            if (dir_raw == ~(uint16_t)0) {
                /* set dir by last measures */
                for (uint8_t i = 0; i < last_count; i++) {
                    if ((dir = last_dirs[i]) != ~(uint16_t)0) {
                        break;
                    }
                }
            } else {
                dir = dir_raw;
            }

            motion_ball(dir);

            for (uint8_t i = last_count - 1; i > 0; i--) {
                last_dirs[i] = last_dirs[i - 1];
            }
            last_dirs[0] = dir_raw;
        }
    }
}

void motion_move(uint8_t dir, uint8_t spd) // 0-8 (not 255)
{
    vic_printf("%u %u\n", dir, stop_next_to_line);
    if (dir == stop_next_to_line) {
        spd = MIN_SPEED;
        stop_next_to_line_time = millis();
    } else {
        uint8_t diff = ((int16_t)stop_next_to_line - dir + 8) % 8;
        if (diff == 1 || diff == 7) {
            dir = (dir + diff) % 8;
            spd = ((uint16_t)spd + MIN_SPEED) / 2;
        } else {
            if (stop_next_to_line != 255
            && millis() - stop_next_to_line_time > 500) { // TODO: constant
                stop_next_to_line = 255;
            }
        }
    }
    WS_SAFE(move_dir(dir, spd); motion_last_dir = dir;);
    if (dir <= 1 || dir >= 7) {
        dribbler_on();
    } else {
        dribbler_off();
    }
}

void motion_line(uint8_t dir)
{
    if (motion_print) {
        lcd.setCursor(0, 1);
        lcd.print("line: ");
        lcd.print(dir);
    }
    static uint32_t last_time = 0;
    uint32_t start_time = millis();
    int16_t line_speed = LINE_SPEED;
    uint32_t line_time = LINE_BASE_TIME + (line_level - 1) * LINE_EXTRA_TIME;
    if (start_time - last_time > 2000) { // TODO: constant
        line_time += 4 * LINE_EXTRA_TIME;
    }
    last_time = start_time;
    vic_printf("set %u\n", stop_next_to_line);
    stop_next_to_line_time = millis();

    switch (dir) {
        case 0:
            while (millis() - start_time < line_time) {
                move_up(line_speed);
            }
            break;
        case 1:
            while (millis() - start_time < line_time) {
                move_up_right(line_speed);
            }
            break;
        case 2:
            while (millis() - start_time < line_time) {
                move_right(line_speed);
            }
            break;
        case 3:
            while (millis() - start_time < line_time) {
                move_back_right(line_speed);
            }
            break;
        case 4:
            while (millis() - start_time < line_time) {
                move_back(line_speed);
            }
            break;
        case 5:
            while (millis() - start_time < line_time) {
                move_back_left(line_speed);
            }
            break;
        case 6:
            while (millis() - start_time < line_time) {
                move_left(line_speed);
            }
            break;
        case 7:
            while (millis() - start_time < line_time) {
                move_up_left(line_speed);
            }
            break;
    }
}

void motion_ball(uint16_t dir)
{
    uint8_t far = 0;
    if (dir != ~(uint16_t)0 && dir >= 1000) {
        far = 1;
        dir -= 1000;
    }

    if (motion_print) {
        lcd.setCursor(0, 1);
        lcd.print("ball ");
        if (dir != ~(uint16_t)0) {
            lcd.print(dir);
            if (far) lcd.print('F');
            else lcd.print('N');
        }
    }
    int16_t speed_near = SPEED_NEAR;
    int16_t speed = SPEED;

    switch (dir) {
        case 0:
            if (ball_in_dribbler()) {
                if (use_pixy == 1) {
                    pixy_motion_goal();
                } else {
                    WS_SAFE(move_up(speed); motion_last_dir = 0);
                    dribbler_kick();
                }
            } else {
                motion_move(0, far ? speed : speed_near);
            }
            ball_side = 'N';
            break;

        case 15:
            motion_move(1, far ? speed : speed_near);
            ball_side = 'R';
            break;

        case 30:
            motion_move(far ? 1 : 2, speed);
            ball_side = 'R';
            break;

        case 60:
            motion_move(far ? 2 : 3, speed);
            ball_side = 'R';
            break;

        case 90:
            motion_move(far ? 2 : 3, speed);
            ball_side = 'R';
            break;

        case 120:
            motion_move(far ? 3 : 4, speed);
            ball_side = 'R';
            break;

        case 150:
            motion_move(far ? 4 : 5, speed);
            ball_side = 'R';
            break;

        case 180:
            motion_move(far ? 4 : (ball_side == 'L' ? 3 : 5), speed);
            break;

        case 210:
            motion_move(far ? 4 : 3, speed);
            ball_side = 'L';
            break;

        case 240:
            motion_move(far ? 5 : 4, speed);
            ball_side = 'L';
            break;

        case 270:
            motion_move(far ? 6 : 5, speed);
            ball_side = 'L';
            break;

        case 300:
            motion_move(far ? 6 : 5, speed);
            ball_side = 'L';
            break;

        case 330:
            motion_move(far ? 7 : 6, speed);
            ball_side = 'L';
            break;

        case 345:
            motion_move(7, far ? speed : speed_near);
            ball_side = 'L';
            break;

        case ~(uint16_t)0:
            halt();
            dribbler_off();
            break;
    }
}

void pixy_motion_ball()
{
    static uint32_t last_get_data = millis();
    if (millis() - last_get_data < 30) {
        return;
    }
    last_get_data = millis();

    pixyViSy.updateBall();
    uint16_t distance = pixyViSy.getDistance();
    int16_t angle = pixyViSy.getBallAngle();
    static uint32_t last_in_front = millis();

    if (distance == ~(uint16_t)0) {
        if (ball_in_dribbler()) {
            distance = 100;
            angle = 0;
        } else if (ball_side == 'N' && millis() - last_in_front < PIXY_TIME) {
            distance = 200;
            angle = 0;
        }
    }

    if (distance == ~(uint16_t)0) {
        if (ball_side == 'L') {
            WS_SAFE(rotate(ROTATE_SPEED));
            compass_set_north();
        } else {
            WS_SAFE(rotate(-ROTATE_SPEED));
            compass_set_north();
        }
    } else {
        uint16_t north_angle = (compass_north() + 360 - start_north) % 360;
        uint16_t pos_angle = (compass_angle_raw() + 360 - start_north) % 360;

#if 0
        if (compass_north() != start_north) {
            if (motion_dbg) {
                Serial.print(north_angle);
            }
            if (north_angle < 180) {
                if (angle < PIXY_FOV / 2 - 20) {
                    compass_set_north_val((compass_north() + 359) & 360);
                }
                /*
                uint16_t ball_rot_angle = 1 + (PIXY_FOV / 2 - angle) / 30;
                if (motion_dbg) {
                    Serial.println(ball_rot_angle);
                }
                compass_set_north_val((compass_north() + 360 -
                    min(north_angle, ball_rot_angle)) % 360);
                */
            } else {
                if (angle > -(PIXY_FOV / 2 - 20)) {
                    compass_set_north_val((compass_north() + 361) & 360);
                }
                /*
                uint16_t ball_rot_angle = 1 + (PIXY_FOV / 2 + angle) / 30;
                if (motion_dbg) {
                    Serial.println(ball_rot_angle);
                }
                compass_set_north_val((compass_north() + 360 +
                    min(360 - north_angle, ball_rot_angle)) % 360);
                */
            }
        }
#endif

        if (angle < -23) {
            WS_SAFE(move_left(PIXY_BALL_SPEED));
            motion_last_dir = 6;
            dribbler_on();
            ball_side = 'L';
        } else if (angle < -8) {
            WS_SAFE(move_left(PIXY_BALL_SPEED_NEAR));
            motion_last_dir = 6;
            dribbler_on();
            ball_side = 'L';
        } else if (angle < 9) {
            if (ball_in_dribbler()) {
                if (compass_north() == start_north) {
                    WS_SAFE(move_up(PIXY_BALL_SPEED));
                    motion_last_dir = 0;
                    dribbler_kick();
                } else {
                    uint16_t diff =
                        (compass_north() + 360 - start_north) % 360;
                    if (diff < TO_NORTH_THRESH) {
                        compass_set_north_val(start_north);
                    } else if (diff < 180) {
                        WS_SAFE(rotate(ROTATE_SPEED));
                        compass_set_north();
                    } else if (diff < 360 - TO_NORTH_THRESH) {
                        WS_SAFE(rotate(-ROTATE_SPEED));
                        compass_set_north();
                    } else {
                        compass_set_north_val(start_north);
                    }
                }
            } else {
                WS_SAFE(move_up(PIXY_BALL_SPEED));
                motion_last_dir = 0;
                ball_side = 'N';
            }
            last_in_front = millis();
        } else if (angle < 24) {
            WS_SAFE(move_right(PIXY_BALL_SPEED_NEAR));
            motion_last_dir = 2;
            dribbler_on();
            ball_side = 'R';
        } else {
            WS_SAFE(move_right(PIXY_BALL_SPEED));
            motion_last_dir = 2;
            dribbler_on();
            ball_side = 'R';
        }

        if (motion_dbg) {
            Serial.print("angle: "); Serial.println(angle);
        }
    }
}

void pixy_motion_goal()
{
    pixyViSy.update();
    char action = pixyViSy.getAction();
    if (motion_print) {
        lcd.setCursor(12, 1);
        lcd.print(action);
    }
    switch (action) {
        case 'L':
            WS_SAFE(move_left(PIXY_SPEED); motion_last_dir = 6);
            dribbler_on();
            goal_dir = 'L';
            break;
        case 'R':
            WS_SAFE(move_right(PIXY_SPEED); motion_last_dir = 2);
            dribbler_on();
            goal_dir = 'R';
            break;
        case 'K':
            WS_SAFE(move_up(SPEED); motion_last_dir = 0);
            dribbler_kick();
            break;
        case 'N':
            if (goal_dir == 'L') {
                WS_SAFE(move_back_left(PIXY_SPEED); motion_last_dir = 5);
            } else {
                WS_SAFE(move_back_right(PIXY_SPEED); motion_last_dir = 3);
            }
            break;
    }
}
