Button btns[BUTTONS_COUNT] =
    { Button(BTN1_PIN), Button(BTN2_PIN), Button(BTN3_PIN), Button(BTN4_PIN) };

inline uint8_t btn_state(uint8_t index)
{
    return btns[index].read();
}

void setup_buttons()
{
    btns[0].set_onPush(btn1_onPush);
    btns[0].set_onRelease(btn1_onRelease);

    btns[1].set_onPush(btn2_onPush);

    btns[2].set_onPush(btn3_onPush);

    btns[3].set_onPush(btn4_onPush);
    btns[3].set_pushed(btn4_pushed);
}

void update_buttons()
{
    static uint32_t time = millis();
    if (millis() - time > BUTTONS_DELAY) {
        for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
            btns[i].update();
        }
        time = millis();
    }
}

/* VIC FUNCTIONS */

void vic_btn_state(void)
{
    uint8_t index;
    if (vic_args("%hhu", &index) == 1) {
        vic_printf("%u\n", btn_state(index));
    }
}

void vic_all_btns_state(void)
{
    for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
        vic_printf("%u", btn_state(i));
        if (i < BUTTONS_COUNT - 1) {
            vic_out(' ');
        }
    }
    vic_out('\n');
}

/* BUTTON ACTIONS */

uint32_t btn1_time;

void btn1_onPush(void)
{
    btn1_time = millis();
}

void btn1_onRelease(void)
{
    if (millis() - btn1_time > 1000) {
        if (task_id == TASK_TEST) {
            task_id = TASK_NO;
            IDLE();
        } else {
            task_id = TASK_TEST;
            test_start();
        }
        return;
    }
    switch (task_id) {
        case TASK_NO:
            task_id = TASK_DISPLAY;
            actual_screen = 1;
            led_set(0, 0);
            break;
        case TASK_DISPLAY:
            task_id = TASK_MOTION;
            motion_start();
            break;
        case TASK_MOTION:
            task_id = TASK_NO;
            IDLE();
            break;
        default:
            break;
    }
}

void btn2_onPush(void)
{
    switch (task_id) {
        case TASK_NO:
            dribbler_switch_wk();
            break;
        case TASK_DISPLAY:
            actual_screen = actual_screen == 4 ? 1 : actual_screen + 1;
            break;
        case TASK_TEST:
            test_dbg = !test_dbg;
            break;
        case TASK_MOTION:
            motion_dbg = !motion_dbg;
            break;
        default:
            break;
    }
}

void btn3_onPush(void)
{
    switch (task_id) {
        case TASK_NO:
            line_use_int = !line_use_int;
            break;
        case TASK_DISPLAY:
            if (actual_screen == 4) {
                light_pwm = (light_pwm + 1) % 256;
            }
            break;
        case TASK_TEST:
            break;
        case TASK_MOTION:
            break;
        default:
            break;
    }
}

void btn4_pushed(void)
{
    if (task_id == TASK_DISPLAY && actual_screen == 4) {
        light_pwm = (light_pwm + 1) % 256;
    }
}

void btn4_onPush(void)
{
    switch (task_id) {
        case TASK_NO:
            use_pixy = (use_pixy + 1) % 3;
            break;
        case TASK_DISPLAY:
            if (actual_screen == 1) {
                compass_set_north();
            }
            break;
        case TASK_TEST:
            compass_set_north();
            break;
        case TASK_MOTION:
            compass_set_north();
            break;
        default:
            break;
    }
}
