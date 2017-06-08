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
    motion_last_dir = 255;
    start_north = compass_north();
    restart_PID();
    line_speed_down = 0;
    mutex = 1;
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++){
        ws[i] = 0;
    }
    mutex = 0;

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

    if (!line_use_int) {
        for (uint8_t i = 0; i < 0; i++) {
            ws[i] = read_line_sensor(i);
        }
    }
    uint16_t dir = process_ws();

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

void motion_line(uint8_t dir)
{
    if (motion_print) {
        lcd.setCursor(0, 1);
        lcd.print("line: ");
        lcd.print(dir);
    }
    uint32_t start_time = millis();
    int16_t line_speed = LINE_SPEED;
    if (line_speed_down) {
        line_speed /= 3;
    }

    switch (dir) {
        case 0:
            while (millis() - start_time < LINE_TIME) {
                move_up(line_speed);
            }
            break;
        case 1:
            while (millis() - start_time < LINE_TIME) {
                move_up_right(line_speed);
            }
            break;
        case 2:
            while (millis() - start_time < LINE_TIME) {
                move_right(line_speed);
            }
            break;
        case 3:
            while (millis() - start_time < LINE_TIME) {
                move_back_right(line_speed);
            }
            break;
        case 4:
            while (millis() - start_time < LINE_TIME) {
                move_back(line_speed);
            }
            break;
        case 5:
            while (millis() - start_time < LINE_TIME) {
                move_back_left(line_speed);
            }
            break;
        case 6:
            while (millis() - start_time < LINE_TIME) {
                move_left(line_speed);
            }
            break;
        case 7:
            while (millis() - start_time < LINE_TIME) {
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
    if (line_speed_down) {
        speed_near /= 3;
        speed /= 3;
    }

    switch (dir) {
        case 0:
            if (ball_in_dribbler()) {
                if (use_pixy == 2) {
                    if (compass_north() == start_north) {
                        WS_SAFE(move_up(speed); motion_last_dir = 0);
                        dribbler_off();
                        kick();
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
                } else if (use_pixy == 1) {
                    pixy_motion_goal();
                } else {
                    WS_SAFE(move_up(speed); motion_last_dir = 0);
                    dribbler_off();
                    kick();
                }
            } else {
                WS_SAFE(move_up(far ? speed : speed_near);motion_last_dir = 0);
                dribbler_on();
            }
            ball_side = 'N';
            break;

        case 15:
            WS_SAFE(move_up_right(far ? SPEED : SPEED_NEAR);motion_last_dir=1);
            dribbler_on();
            ball_side = 'R';
            break;

        case 30:
            if (far) {
                WS_SAFE(move_up_right(speed); motion_last_dir = 1);
            } else {
                WS_SAFE(move_right(speed); motion_last_dir = 2);
            }
            dribbler_on();
            ball_side = 'R';
            break;

        case 60:
            if (far) {
                WS_SAFE(move_right(speed); motion_last_dir = 2);
            } else {
                WS_SAFE(move_back_right(speed); motion_last_dir = 3);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 90:
            if (far) {
                WS_SAFE(move_right(speed); motion_last_dir = 2);
            } else {
                WS_SAFE(move_back_right(speed); motion_last_dir = 3);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 120:
            if (far) {
                WS_SAFE(move_back_right(speed); motion_last_dir = 3);
            } else {
                WS_SAFE(move_back(speed); motion_last_dir = 4);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 150:
            if (far) {
                WS_SAFE(move_back(speed); motion_last_dir = 4);
            } else {
                WS_SAFE(move_back_left(speed); motion_last_dir = 5);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 180:
            if (far) {
                WS_SAFE(move_back(speed); motion_last_dir = 4);
            } else if (ball_side == 'L') {
                WS_SAFE(move_back_right(speed); motion_last_dir = 3);
            } else {
                WS_SAFE(move_back_left(speed); motion_last_dir = 5);
            }
            dribbler_off();
            break;

        case 210:
            if (far) {
                WS_SAFE(move_back(speed); motion_last_dir = 4);
            } else {
                WS_SAFE(move_back_right(speed); motion_last_dir = 3);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 240:
            if (far) {
                WS_SAFE(move_back_left(speed); motion_last_dir = 5);
            } else {
                WS_SAFE(move_back(speed); motion_last_dir = 4);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 270:
            if (far) {
                WS_SAFE(move_left(speed); motion_last_dir = 6);
            } else {
                WS_SAFE(move_back_left(speed); motion_last_dir = 5);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 300:
            if (far) {
                WS_SAFE(move_left(speed); motion_last_dir = 6);
            } else {
                WS_SAFE(move_back_left(speed); motion_last_dir = 5);
            }
            dribbler_on();
            ball_side = 'L';
            break;

        case 330:
            if (far) {
                WS_SAFE(move_up_left(speed); motion_last_dir = 7);
            } else {
                WS_SAFE(move_left(speed); motion_last_dir = 6);
            }
            dribbler_on();
            ball_side = 'L';
            break;

        case 345:
            WS_SAFE(move_up_left(far ? SPEED : SPEED_NEAR);motion_last_dir=7);
            dribbler_on();
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
                    dribbler_off();
                    kick();
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
            dribbler_off();
            kick();
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
