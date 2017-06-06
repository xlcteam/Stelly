# 1 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
# 2 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 3 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 4 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 5 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 6 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 7 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 8 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 9 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 10 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 11 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino" 2
# 60 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
/* Arduino puts function prototypes from all ino files "after any comments or
   pre-processor statements (#includes or #defines), but before any other
   statements (including type declarations)."
   So below there is no special function, it actually does nothing, but
   'lets' arduino to generate function prototypes here.
*/
void Arduino_IDE_will_generate_function_prototypes_here() {return;}

uint8_t line_use_int = 1;
uint8_t use_pixy = 0 /* 1 - goal; 2 - ball */;
uint8_t light_pwm = 130 /*130*/;
uint8_t actual_screen;
uint8_t motion_dbg = 0;
uint8_t test_dbg = 0;
uint32_t last_time;
uint16_t delta_time;
uint8_t motion_last_dir;
uint8_t line_speed_down;

PixyViSy pixyViSy(1, 120, 75);

void (*tasks[4])(void) = {
    __null,
    print_display_screen,
    test_loop,
    motion
};
uint8_t task_id = 0;

void vic_camera_data(void)
{
    if (use_pixy == 1) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%c %u\n"), pixyViSy.getAction(), pixyViSy.getDistance()); vic_print(vic_buffer_out); } while(0);
    } else if (use_pixy == 2) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%i %u\n"), pixyViSy.getBallAngle(), pixyViSy.getDistance()); vic_print(vic_buffer_out); } while(0);
    }
}

void IDLE(void)
{
    print_display_idle();
    led_set(0, 0);
    halt();
    dribbler_off();
}

uint32_t volatile ws[] = {0, 0, 0};
uint8_t mutex[] = {0, 0, 0};

extern "C" void __vector_9 (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_9 (void)
{
    if (!mutex[0] && (*(volatile uint8_t *)((0X03) + 0x20)) & (1 << (5)) && !ws[0]) {
        ws[0] = micros();
        halt();
    }

    if (!mutex[1] && (*(volatile uint8_t *)((0X03) + 0x20)) & (1 << (7)) && !ws[1]) {
        ws[1] = micros();
        halt();
    }

    if (!mutex[2] && (*(volatile uint8_t *)((0X03) + 0x20)) & (1 << (4)) && !ws[2]) {
        ws[2] = micros();
        halt();
    }
}

void serial_output(char c)
{
    Serial.print(c);
}


void setup()
{
    Serial.begin(115200);
    Wire.begin();
    __asm__ __volatile__ ("sei" ::: "memory"); /* enables interrupts */

    vic_init(serial_output);
    setup_vic_funcs();
    setup_vic_vars();

    compass_set_north();

    setup_buttons();
    setup_IR_sensors();
    setup_dribbler();
    setup_display();
    print_display_idle();
    setup_line_sensors();

    last_time = millis();
}

void loop()
{
    while (Serial.available()) {
        vic_process((char)Serial.read());
    }

    update_buttons();

    if (tasks[task_id] != __null) {
        tasks[task_id]();
    }

    line_sensors_update();

    led_set(3, use_pixy);
    led_set(1, use_pixy == 2);
    pixyViSy.setBallSize(use_pixy == 2 ? 67 : 75);

    delta_time = millis() - last_time;
    last_time = millis();
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint8_t IR_sensors_pins[14] = { A5, A10, A11,
    A12, A13, A14, A15, A0, A1, A2, A3,
    A7, A4, A6 };
uint16_t IR_vals_far[14] =
    { 813, 1008, 1009, 1011, 1014, 1015, 1017,
      1016, 1016, 1015, 1012, 1015, 1015, 1013 };
uint16_t IR_vals_near[14] =
    { 116, 114, 115, 117, 117, 116, 117,
      115, 109, 111, 113, 117, 140, 163 };

void setup_IR_sensors()
{
    for (uint8_t i = 0; i < 14; i++) {
        pinMode(IR_sensors_pins[i], 0x0);
    }
}

inline uint16_t read_IR_raw(uint8_t index)
{
    return analogRead(IR_sensors_pins[index]);
}

uint16_t read_IR(uint8_t i)
{
    uint16_t val = read_IR_raw(i);
    val = ((val)<(IR_vals_near[i])?(IR_vals_near[i]):((val)>(IR_vals_far[i])?(IR_vals_far[i]):(val)));
    return map(val, IR_vals_near[i], IR_vals_far[i], 0, 1024);
}

void read_IR_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < 14; i++) {
        out_data[i] = read_IR(i);
    }
}

void read_IR_raw_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < 14; i++) {
        out_data[i] = read_IR_raw(i);
    }
}

uint8_t max_IR()
{
    uint16_t data[14];
    uint16_t max_index = 0;

    read_IR_all(data);
    for (uint8_t i = 0; i < 14 - 2; i++) { // last 2 are special
        if (data[i] < data[max_index]) {
            max_index = i;
        }
    }

    return max_index;
}

/* returns angle to ball (if far, then increased by 1000)
   ~0: no ball
*/
uint16_t dir_to_ball()
{
    uint16_t data[14];
    uint8_t nearest = 0;
    read_IR_all(data);

    for (uint8_t i = 0; i < 14 - 2; i++) {
        if (data[i] < data[nearest]) {
            nearest = i;
        }
    }

    if (data[nearest] > 1000) {
        return ~0;
    }

    if (ball_in_dribbler() && data[0] < 1000) {
        return 0;
    }

    if (data[nearest] > 550) {
        return 1000 + nearest * 30;
    }

    if (nearest == 1) {
        if (data[12] > 550) {
            return 30;
        } else {
            return 15;
        }
    } else if (nearest == 11) {
        if (data[13] > 550) {
            return 330;
        } else {
            return 345;
        }
    }
    return nearest * 30;
}

/* VIC FUNCTIONS */

void vic_read_IR(void)
{
    uint8_t index;
    if ((sscanf(vic_args_s, ("%hhu"), &index)) == 1) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), read_IR(index)); vic_print(vic_buffer_out); } while(0);
    }
}

void vic_read_IR_raw(void)
{
    uint8_t index;
    if ((sscanf(vic_args_s, ("%hhu"), &index)) == 1) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), read_IR_raw(index)); vic_print(vic_buffer_out); } while(0);
    }
}

void vic_read_IR_all(void)
{
    uint16_t data[14];
    read_IR_all(data);
    for (uint8_t i = 0; i < 14 - 1; i++) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%4u "), data[i]); vic_print(vic_buffer_out); } while(0);
    }
    do { snprintf(vic_buffer_out, 128 + 1, ("%4u\n"), data[14 - 1]); vic_print(vic_buffer_out); } while(0);
}

void vic_read_IR_raw_all(void)
{
    uint16_t data[14];
    read_IR_raw_all(data);
    for (uint8_t i = 0; i < 14 - 1; i++) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%4u "), data[i]); vic_print(vic_buffer_out); } while(0);
    }
    do { snprintf(vic_buffer_out, 128 + 1, ("%4u\n"), data[14 - 1]); vic_print(vic_buffer_out); } while(0);
}

void vic_dir_to_ball(void)
{
    uint16_t dir = dir_to_ball();
    if (dir == ~(uint16_t)0) {
        do { vic_print("no ball"); vic_out('\n'); } while(0);
    } else if (dir > 1000) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u far\n"), dir - 1000); vic_print(vic_buffer_out); } while(0);
    } else {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u near\n"), dir); vic_print(vic_buffer_out); } while(0);
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
Button btns[4] =
    { Button(27), Button(25), Button(24), Button(37) };

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
    if (millis() - time > 10) {
        for (uint8_t i = 0; i < 4; i++) {
            btns[i].update();
        }
        time = millis();
    }
}

/* VIC FUNCTIONS */

void vic_btn_state(void)
{
    uint8_t index;
    if ((sscanf(vic_args_s, ("%hhu"), &index)) == 1) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), btn_state(index)); vic_print(vic_buffer_out); } while(0);
    }
}

void vic_all_btns_state(void)
{
    for (uint8_t i = 0; i < 4; i++) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u"), btn_state(i)); vic_print(vic_buffer_out); } while(0);
        if (i < 4 - 1) {
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
        if (task_id == 2) {
            task_id = 0;
            IDLE();
        } else {
            task_id = 2;
            test_start();
        }
        return;
    }
    switch (task_id) {
        case 0:
            task_id = 1;
            actual_screen = 1;
            led_set(0, 0);
            break;
        case 1:
            task_id = 3;
            motion_start();
            break;
        case 3:
            task_id = 0;
            IDLE();
            break;
        default:
            break;
    }
}

void btn2_onPush(void)
{
    switch (task_id) {
        case 0:
            dribbler_switch_wk();
            break;
        case 1:
            actual_screen = actual_screen == 4 ? 1 : actual_screen + 1;
            break;
        case 2:
            test_dbg = !test_dbg;
            break;
        case 3:
            motion_dbg = !motion_dbg;
            break;
        default:
            break;
    }
}

void btn3_onPush(void)
{
    switch (task_id) {
        case 0:
            line_use_int = !line_use_int;
            break;
        case 1:
            if (actual_screen == 4) {
                light_pwm = (light_pwm + 1) % 256;
            }
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }
}

void btn4_pushed(void)
{
    if (task_id == 1 && actual_screen == 4) {
        light_pwm = (light_pwm + 1) % 256;
    }
}

void btn4_onPush(void)
{
    switch (task_id) {
        case 0:
            use_pixy = (use_pixy + 1) % 3;
            break;
        case 1:
            if (actual_screen == 1) {
                compass_set_north();
            }
            break;
        case 2:
            compass_set_north();
            break;
        case 3:
            compass_set_north();
            break;
        default:
            break;
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
RelativeHMC6352 compass = RelativeHMC6352();

inline uint16_t compass_angle(void)
{
    return (uint16_t) compass.angle();
}

inline uint16_t compass_angle_raw(void)
{
    return (uint16_t) compass.angle_raw();
}

inline uint16_t compass_north(void)
{
    return (uint16_t) compass.real_north();
}

inline void compass_set_north_val(uint16_t value)
{
    compass.set_north_val((float)value);
}

inline void compass_set_north(void)
{
    compass.set_north();
}

bool centered(uint16_t range)
{
    uint16_t c = (uint16_t) compass_angle();
    return ((c >= (360 - range)) || (c <= range));
}

/* VIC FUNCTIONS */

inline void vic_compass_angle(void)
{
    do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), compass_angle()); vic_print(vic_buffer_out); } while(0);
}

inline void vic_compass_north(void)
{
    do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), (uint16_t) compass_north()); vic_print(vic_buffer_out); } while(0);
}

inline void vic_compass_set_north(void)
{
    compass_set_north();
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
LiquidCrystal lcd(
    32, 33, 35, 29, 30, 31);

void setup_display()
{
    lcd.begin(16, 2);
    actual_screen = 0;
}

void print_display_idle()
{
    delay(100);
    lcd.clear();
    lcd.display();

    lcd.print("------Idle------");
}

void print_display_1(void)
{
    lcd.print("IR   L C   D P");
    lcd.setCursor(0, 1);
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
            lcd.print('F');
        } else {
            lcd.print(dir);
            lcd.print('N');
        }
    }
    lcd.setCursor(5, 1);
    lcd.print(line_for_display());
    lcd.setCursor(7, 1);
    lcd.print(compass_angle());
    lcd.setCursor(11, 1);
    lcd.print(ball_in_dribbler());
    lcd.setCursor(13, 1);
    if (use_pixy == 1) {
        pixyViSy.update();
        lcd.print(pixyViSy.getAction());
    } else if (use_pixy == 2) {
        pixyViSy.updateBall();
        lcd.print(pixyViSy.getBallAngle());
    }
}

void print_display_2(void)
{
    lcd.print("IR");
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
        } else {
            lcd.print(dir);
        }
    }
    lcd.setCursor(5, 0);
    lcd.print(((read_IR(0))<(999)?(read_IR(0)):(999)));
    lcd.setCursor(9, 0);
    lcd.print(((read_IR(1))<(999)?(read_IR(1)):(999)));
    lcd.setCursor(13, 0);
    lcd.print(((read_IR(2))<(999)?(read_IR(2)):(999)));
    lcd.setCursor(0, 1);
    lcd.print(((read_IR(3))<(999)?(read_IR(3)):(999)));
    lcd.setCursor(4, 1);
    lcd.print(((read_IR(4))<(999)?(read_IR(4)):(999)));
    lcd.setCursor(8, 1);
    lcd.print(((read_IR(5))<(999)?(read_IR(5)):(999)));
    lcd.setCursor(12, 1);
    lcd.print(((read_IR(6))<(999)?(read_IR(6)):(999)));
}

void print_display_3(void)
{
    lcd.print("->");
    uint16_t dir = dir_to_ball();
    if (dir != ~(uint16_t)0) {
        if (dir >= 1000) {
            lcd.print(dir - 1000);
        } else {
            lcd.print(dir);
        }
    }
    lcd.setCursor(5, 0);
    lcd.print(((read_IR(7))<(999)?(read_IR(7)):(999)));
    lcd.setCursor(9, 0);
    lcd.print(((read_IR(8))<(999)?(read_IR(8)):(999)));
    lcd.setCursor(13, 0);
    lcd.print(((read_IR(9))<(999)?(read_IR(9)):(999)));
    lcd.setCursor(0, 1);
    lcd.print(((read_IR(10))<(999)?(read_IR(10)):(999)));
    lcd.setCursor(4, 1);
    lcd.print(((read_IR(11))<(999)?(read_IR(11)):(999)));
    lcd.setCursor(8, 1);
    lcd.print(((read_IR(12))<(999)?(read_IR(12)):(999)));
    lcd.setCursor(12, 1);
    lcd.print(((read_IR(13))<(999)?(read_IR(13)):(999)));
}

void print_display_4(void)
{
    lcd.print("WL  1 2 3");
    lcd.setCursor(0, 1);
    lcd.print(light_pwm);
    uint8_t i;
    for (i = 0; i < 3; i++) {
        lcd.setCursor(4 + 2 * i, 1);
        lcd.print(read_line_sensor(i));
    }
    lcd.setCursor(4 + 2 * i, 0);
    lcd.print("D");
    lcd.setCursor(4 + 2 * i, 1);
    lcd.print(line_sensors_dir());
}

void print_display_screen(void)
{
    static uint32_t time = millis();
    if (millis() - time > 100) {
        lcd.clear();
        lcd.display();

        switch (actual_screen) {
            case 1:
                print_display_1();
                break;
            case 2:
                print_display_2();
                break;
            case 3:
                print_display_3();
                break;
            case 4:
                print_display_4();
                break;
            default:
                lcd.print("SCREEN ERROR");
                break;
        }
        time = millis();
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
Motor dribbler = Motor(23, 2);
int8_t dribbler_state;

void setup_dribbler()
{
    pinMode(28, 0x1);
    pinMode(44, 0x0);
    dribbler_off();
}

inline void dribbler_off()
{
    dribbler.stop();
    dribbler_state = 0;
}

inline void dribbler_on()
{
    dribbler.go(-255); //255
    dribbler_state = 1;
}

inline void dribbler_switch()
{
    dribbler.go(255 * (dribbler_state = !dribbler_state));
}

/* with kick */
void dribbler_switch_wk()
{
    if (dribbler_state == 1)
        dribbler_kick();
    else if (dribbler_state == -1)
        dribbler_off();
    else if (dribbler_state == 0)
        dribbler_on();
}

inline void dribbler_kick()
{
    dribbler.go(-255); //-255 ,255-2.
    dribbler_state = -1;
}

bool ball_in_dribbler()
{
    static uint32_t l_time = millis();
    if (!digitalRead(44)) {
        l_time = millis();
        return true;
    }
    if (millis() - l_time < 20) {
        return true;
    }
    return false;
}

/* VIC FUNCTIONS */

void vic_dribbler_off(void)
{
    dribbler_off();
}

void vic_dribbler_on(void)
{
    dribbler_on();
}

void vic_dribbler_kick(void)
{
    dribbler_kick();
}

void vic_ball_in_dribbler(void)
{
    do { snprintf(vic_buffer_out, 128 + 1, ("%u\n"), (uint8_t) ball_in_dribbler()); vic_print(vic_buffer_out); } while(0);
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
uint8_t leds[4] = { 47, 49, 43, 45 };

void setup_leds()
{
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(leds[i], 0x1);
    }
}

inline void led_set(uint8_t index, uint8_t state)
{
    digitalWrite(leds[index], state);
}

void leds_set_all(uint8_t states)
{
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(leds[i], states & (0x01 << i));
    }
}

/* VIC FUNCTIONS */

void vic_led_set(void)
{
    uint8_t index, state;
    if ((sscanf(vic_args_s, ("%hhu %hhu"), &index, &state)) == 2) {
        led_set(index, state);
    }
}

void vic_leds_set_all(void)
{
    uint8_t states;
    if ((sscanf(vic_args_s, ("%hhu"), &states)) == 1) {
        leds_set_all(states);
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
uint8_t line_sensors[3] =
    { 11, 13, 10 };

void setup_line_sensors()
{
    for (uint8_t i = 0; i < 3; i++) {
        pinMode(line_sensors[i], 0x0);
    }
    pinMode(9, 0x1);

    (*(volatile uint8_t *)(0x6B)) |= (1 << (4)) | (1 << (5)) | (1 << (6)) | (1 << (7));

    line_sensors_update();
}

inline uint8_t read_line_sensor(uint8_t index)
{
    return digitalRead(line_sensors[index]);
}

void line_sensors_update()
{
    analogWrite(9, light_pwm);
    (*(volatile uint8_t *)(0x68)) &= ~(1 << (0)); // clear
    (*(volatile uint8_t *)(0x68)) |= (line_use_int & 0x01) << 0; // set
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
        if (motion_last_dir < 180 && motion_last_dir > 15) {
            dir = 5;
        } else if (motion_last_dir > 180 && motion_last_dir < 345) {
            dir = 3;
        } else {
            dir = 4;
        }
    } else if (ws[0] && ws[1] && !ws[2]) {
        if ((((int32_t)ws[0] - (int32_t)ws[1])>0?((int32_t)ws[0] - (int32_t)ws[1]):-((int32_t)ws[0] - (int32_t)ws[1])) < 10000) {
            dir = 5;
        } else if (ws[0] < ws[1]) {
            dir = 5;
        } else { /* ws[1] < ws[0] */
            dir = 6;
        }
    } else if (ws[0] && !ws[1] && ws[2]) {
        if ((((int32_t)ws[0] - (int32_t)ws[2])>0?((int32_t)ws[0] - (int32_t)ws[2]):-((int32_t)ws[0] - (int32_t)ws[2])) < 10000) {
            dir = 3;
        } else if (ws[0] < ws[2]) {
            dir = 3;
        } else { /* ws[2] < ws[0] */
            dir = 2;
        }
    } else if (!ws[0] && ws[1] && ws[2]) {
        dir = 0;
    } else if (!ws[0] && !ws[1] && ws[2]) {
        if (motion_last_dir > 180 || motion_last_dir == 0) {
            dir = 2;
        } else {
            dir = 0;
        }
    } else if (!ws[0] && ws[1] && !ws[2]) {
        if (motion_last_dir >= 0 && motion_last_dir < 180) {
            dir = 6;
        } else {
            dir = 0;
        }
    } else { /* (ws[0] && ws[1] && ws[2]) */

        if ((((int32_t)ws[0] - (int32_t)ws[1])>0?((int32_t)ws[0] - (int32_t)ws[1]):-((int32_t)ws[0] - (int32_t)ws[1])) < 10000
        && (((int32_t)ws[0] - (int32_t)ws[2])>0?((int32_t)ws[0] - (int32_t)ws[2]):-((int32_t)ws[0] - (int32_t)ws[2])) < 10000
        && (((int32_t)ws[1] - (int32_t)ws[2])>0?((int32_t)ws[1] - (int32_t)ws[2]):-((int32_t)ws[1] - (int32_t)ws[2])) < 10000) {
            dir = line_last_dir;
        } else if ((((int32_t)ws[0] - (int32_t)ws[1])>0?((int32_t)ws[0] - (int32_t)ws[1]):-((int32_t)ws[0] - (int32_t)ws[1])) < 10000) {
            if (ws[2] < ws[0]) {
                dir = 1;
            } else {
                dir = 5;
            }
        } else if ((((int32_t)ws[0] - (int32_t)ws[2])>0?((int32_t)ws[0] - (int32_t)ws[2]):-((int32_t)ws[0] - (int32_t)ws[2])) < 10000) {
            if (ws[1] < ws[0]) {
                dir = 7;
            } else {
                dir = 3;
            }
        } else if ((((int32_t)ws[1] - (int32_t)ws[2])>0?((int32_t)ws[1] - (int32_t)ws[2]):-((int32_t)ws[1] - (int32_t)ws[2])) < 10000) {
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
# 152 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
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
                min_time = 400;
                dir = motion_last_dir;
                line_speed_down = 1;
            } else {
                Serial.print("ws0");
                state = 2;
                min_time = 400 / 2;
                dir = 4;
            }
            d_time = s_time = millis();
        } else {
            state = 2;
            min_time = (count == 2) ? 400 : 400 * 2;
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
            if (millis() - d_time > 100
            && millis() - s_time > min_time) {
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
    for (uint8_t i = 0; i < 3; i++) {
        out |= read_line_sensor(i) << i;
    }
    return out;
}

/* VIC FUNCTIONS */

void vic_print_line_sensors(void)
{
    for (uint8_t i = 0; i < 3; i++) {
        do { snprintf(vic_buffer_out, 128 + 1, ("%u "), read_line_sensor(i)); vic_print(vic_buffer_out); } while(0);
    }
    vic_out('\n');
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
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
    line_speed_down = 0;
    if (line_use_int) {
        for (uint8_t i = 0; i < 3; i++){
            mutex[i] = 1;
            ws[i] = 0;
            mutex[i] = 0;
        }
    }
    lcd.clear();
    lcd.print("motion");
    led_set(0, 1);
}

void motion()
{
    static uint32_t time = millis();
    motion_print = motion_dbg && millis() - time > 100;
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

void motion_line(uint8_t dir)
{
    if (motion_print) {
        lcd.setCursor(0, 1);
        lcd.print("line: ");
        lcd.print(dir);
    }
    uint32_t start_time = millis();
    int16_t line_speed = 200;
    if (line_speed_down) {
        line_speed /= 3;
    }

    switch (dir) {
        case 0:
            while (millis() - start_time < 10) {
                move_up(line_speed);
            }
            break;
        case 1:
            while (millis() - start_time < 10) {
                move_up_right(line_speed);
            }
            break;
        case 2:
            while (millis() - start_time < 10) {
                move_right(line_speed);
            }
            break;
        case 3:
            while (millis() - start_time < 10) {
                move_back_right(line_speed);
            }
            break;
        case 4:
            while (millis() - start_time < 10) {
                move_back(line_speed);
            }
            break;
        case 5:
            while (millis() - start_time < 10) {
                move_back_left(line_speed);
            }
            break;
        case 6:
            while (millis() - start_time < 10) {
                move_left(line_speed);
            }
            break;
        case 7:
            while (millis() - start_time < 10) {
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
    int16_t speed_near = 160 /* 160*/;
    int16_t speed = 180 /* 180*/;
    if (line_speed_down) {
        speed_near /= 3;
        speed /= 3;
    }

    switch (dir) {
        case 0:
            if (ball_in_dribbler()) {
                if (use_pixy == 2) {
                    if (compass_north() == start_north) {
                        do { if (!(ws[0] || ws[1] || ws[2])) {move_up(speed); motion_last_dir = 0;} } while (0);
                        dribbler_kick();
                    } else {
                        uint16_t diff =
                            (compass_north() + 360 - start_north) % 360;
                        if (diff < 30) {
                            compass_set_north_val(start_north);
                        } else if (diff < 180) {
                            do { if (!(ws[0] || ws[1] || ws[2])) {rotate(70);} } while (0);
                            compass_set_north();
                        } else if (diff < 360 - 30) {
                            do { if (!(ws[0] || ws[1] || ws[2])) {rotate(-70);} } while (0);
                            compass_set_north();
                        } else {
                            compass_set_north_val(start_north);
                        }
                    }
                } else if (use_pixy == 1) {
                    pixy_motion_goal();
                } else {
                    do { if (!(ws[0] || ws[1] || ws[2])) {move_up(speed); motion_last_dir = 0;} } while (0);
                    dribbler_kick();
                }
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_up(far ? speed : speed_near);motion_last_dir = 0;} } while (0);
                dribbler_on();
            }
            ball_side = 'N';
            break;

        case 15:
            do { if (!(ws[0] || ws[1] || ws[2])) {move_up_right(far ? 180 /* 180*/ : 160 /* 160*/);motion_last_dir=1;} } while (0);
            dribbler_on();
            ball_side = 'R';
            break;

        case 30:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_up_right(speed); motion_last_dir = 1;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_right(speed); motion_last_dir = 2;} } while (0);
            }
            dribbler_on();
            ball_side = 'R';
            break;

        case 60:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_right(speed); motion_last_dir = 2;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(speed); motion_last_dir = 3;} } while (0);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 90:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_right(speed); motion_last_dir = 2;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(speed); motion_last_dir = 3;} } while (0);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 120:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(speed); motion_last_dir = 3;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back(speed); motion_last_dir = 4;} } while (0);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 150:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back(speed); motion_last_dir = 4;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(speed); motion_last_dir = 5;} } while (0);
            }
            dribbler_off();
            ball_side = 'R';
            break;

        case 180:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back(speed); motion_last_dir = 4;} } while (0);
            } else if (ball_side == 'L') {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(speed); motion_last_dir = 3;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(speed); motion_last_dir = 5;} } while (0);
            }
            dribbler_off();
            break;

        case 210:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back(speed); motion_last_dir = 4;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(speed); motion_last_dir = 3;} } while (0);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 240:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(speed); motion_last_dir = 5;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back(speed); motion_last_dir = 4;} } while (0);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 270:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_left(speed); motion_last_dir = 6;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(speed); motion_last_dir = 5;} } while (0);
            }
            dribbler_off();
            ball_side = 'L';
            break;

        case 300:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_left(speed); motion_last_dir = 6;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(speed); motion_last_dir = 5;} } while (0);
            }
            dribbler_on();
            ball_side = 'L';
            break;

        case 330:
            if (far) {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_up_left(speed); motion_last_dir = 7;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_left(speed); motion_last_dir = 6;} } while (0);
            }
            dribbler_on();
            ball_side = 'L';
            break;

        case 345:
            do { if (!(ws[0] || ws[1] || ws[2])) {move_up_left(far ? 180 /* 180*/ : 160 /* 160*/);motion_last_dir=7;} } while (0);
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
        } else if (ball_side == 'N' && millis() - last_in_front < 20) {
            distance = 200;
            angle = 0;
        }
    }

    if (distance == ~(uint16_t)0) {
        if (ball_side == 'L') {
            do { if (!(ws[0] || ws[1] || ws[2])) {rotate(70);} } while (0);
            compass_set_north();
        } else {
            do { if (!(ws[0] || ws[1] || ws[2])) {rotate(-70);} } while (0);
            compass_set_north();
        }
    } else {
        uint16_t north_angle = (compass_north() + 360 - start_north) % 360;
        uint16_t pos_angle = (compass_angle_raw() + 360 - start_north) % 360;
# 390 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
        if (angle < -23) {
            do { if (!(ws[0] || ws[1] || ws[2])) {move_left(150);} } while (0);
            motion_last_dir = 6;
            dribbler_on();
            ball_side = 'L';
        } else if (angle < -8) {
            do { if (!(ws[0] || ws[1] || ws[2])) {move_left(130);} } while (0);
            motion_last_dir = 6;
            dribbler_on();
            ball_side = 'L';
        } else if (angle < 9) {
            if (ball_in_dribbler()) {
                if (compass_north() == start_north) {
                    do { if (!(ws[0] || ws[1] || ws[2])) {move_up(150);} } while (0);
                    motion_last_dir = 0;
                    dribbler_kick();
                } else {
                    uint16_t diff =
                        (compass_north() + 360 - start_north) % 360;
                    if (diff < 30) {
                        compass_set_north_val(start_north);
                    } else if (diff < 180) {
                        do { if (!(ws[0] || ws[1] || ws[2])) {rotate(70);} } while (0);
                        compass_set_north();
                    } else if (diff < 360 - 30) {
                        do { if (!(ws[0] || ws[1] || ws[2])) {rotate(-70);} } while (0);
                        compass_set_north();
                    } else {
                        compass_set_north_val(start_north);
                    }
                }
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_up(150);} } while (0);
                motion_last_dir = 0;
                ball_side = 'N';
            }
            last_in_front = millis();
        } else if (angle < 24) {
            do { if (!(ws[0] || ws[1] || ws[2])) {move_right(130);} } while (0);
            motion_last_dir = 2;
            dribbler_on();
            ball_side = 'R';
        } else {
            do { if (!(ws[0] || ws[1] || ws[2])) {move_right(150);} } while (0);
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
            do { if (!(ws[0] || ws[1] || ws[2])) {move_left(150); motion_last_dir = 6;} } while (0);
            dribbler_on();
            goal_dir = 'L';
            break;
        case 'R':
            do { if (!(ws[0] || ws[1] || ws[2])) {move_right(150); motion_last_dir = 2;} } while (0);
            dribbler_on();
            goal_dir = 'R';
            break;
        case 'K':
            do { if (!(ws[0] || ws[1] || ws[2])) {move_up(180 /* 180*/); motion_last_dir = 0;} } while (0);
            dribbler_kick();
            break;
        case 'N':
            if (goal_dir == 'L') {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_left(150); motion_last_dir = 5;} } while (0);
            } else {
                do { if (!(ws[0] || ws[1] || ws[2])) {move_back_right(150); motion_last_dir = 3;} } while (0);
            }
            break;
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
Motor motorA = Motor(48, 7);
Motor motorB = Motor(41, 3);
Motor motorC = Motor(46, 8);

void brake(void)
{
    motorA.off();
    motorB.off();
    motorC.off();
}

void halt(void)
{
    motorA.stop();
    motorB.stop();
    motorC.stop();
}

int16_t spd_to_north(int16_t speeds[3])
{
    int16_t angle = compass_angle();
    if (angle > 180) angle -= 360;

    int16_t max_speed, max_diff, compensation;

    max_speed = ((speeds[0])>0?(speeds[0]):-(speeds[0]));
    if (((speeds[1])>0?(speeds[1]):-(speeds[1])) > max_speed) { max_speed = ((speeds[1])>0?(speeds[1]):-(speeds[1])); }
    if (((speeds[2])>0?(speeds[2]):-(speeds[2])) > max_speed) { max_speed = ((speeds[2])>0?(speeds[2]):-(speeds[2])); }

    max_diff = 255 - max_speed;

    if (angle > 30) {
        compensation = 70;
    } else if (angle < -30) {
        compensation = -70;
    } else {
        compensation = 70 * angle / 30;
        compensation = compensation * ((angle)>0?(angle):-(angle)) / 30;
    }

    return ((compensation)<(-max_diff)?(-max_diff):((compensation)>(max_diff)?(max_diff):(compensation)));
}

void move_raw(int16_t speeds[3])
{
    motorA.go(speeds[0]);
    motorB.go(speeds[1]);
    motorC.go(speeds[2]);
}

void move_to_north(int16_t speeds[3])
{
    int16_t compensation = spd_to_north(speeds);
    motorA.go(speeds[0] + compensation);
    motorB.go(speeds[1] + compensation);
    motorC.go(speeds[2] + compensation);
}

inline void move_up(uint8_t spd)
{
    int16_t speeds[] = { -spd, spd, 0 };
    move_to_north(speeds);
}

inline void move_right(uint8_t spd)
{
    int16_t speeds[] = { -spd/2, -spd/2, spd };
    move_to_north(speeds);
}

inline void move_back(uint8_t spd)
{
    int16_t speeds[] = { spd, -spd, 0 };
    move_to_north(speeds);
}

inline void move_left(uint8_t spd)
{
    int16_t speeds[] = { spd/2, spd/2, -spd };
    move_to_north(speeds);
}

inline void move_up_right(uint8_t spd)
{
    int16_t speeds[] = { -spd, 0, spd };
    move_to_north(speeds);
}

inline void move_back_right(uint8_t spd)
{
    int16_t speeds[] = { 0, -spd, spd };
    move_to_north(speeds);
}

inline void move_back_left(uint8_t spd)
{
    int16_t speeds[] = { spd, 0, -spd };
    move_to_north(speeds);
}

inline void move_up_left(uint8_t spd)
{
    int16_t speeds[] = { 0, spd, -spd };
    move_to_north(speeds);
}

inline void rotate(int16_t spd)
{
    int16_t speeds[] = { spd, spd, spd };
    move_raw(speeds);
}

/* may be dangerous */
inline void centralize()
{
    int16_t spds[] = {0};
    move_to_north(spds);
}

/* VIC FUNCTIONS */

void vic_move_to_north(void)
{
    int16_t spds[3];

    if ((sscanf(vic_args_s, ("%d %d %d"), &spds[0], &spds[1], &spds[2])) == 3) {
        move_to_north(spds);
    } else {
        vic_print_err(3);
    }
}

void vic_mA(void) { int16_t s; (sscanf(vic_args_s, ("%d"), &s)); motorA.go(s); }
void vic_mB(void) { int16_t s; (sscanf(vic_args_s, ("%d"), &s)); motorB.go(s); }
void vic_mC(void) { int16_t s; (sscanf(vic_args_s, ("%d"), &s)); motorC.go(s); }

void vic_move_up(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_up(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_right(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_right(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_back(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_back(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_left(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_left(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_up_right(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_up_right(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_back_right(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_back_right(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_back_left(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_back_left(spd);
    } else {
        vic_print_err(3);
    }
}

void vic_move_up_left(void)
{
    uint8_t spd;
    if ((sscanf(vic_args_s, ("%hhu"), &spd)) == 1) {
        move_up_left(spd);
    } else {
        vic_print_err(3);
    }
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/pixy_motion_ball.ino"
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/test.ino"
void test_start()
{
    compass_set_north();
    lcd.clear();
    lcd.print("test");
    led_set(0, 0);
    Serial.println("test_start");
}

void test_motors()
{
    static uint8_t action = 1;
    static uint32_t time = millis();
    if (action == 1)
        move_up(180 /* 180*/);
    else if (action == 2)
        move_back(180 /* 180*/);
    else if (action == 3)
        move_left(180 /* 180*/);
    else
        move_right(180 /* 180*/);
    if (millis() - time > 1000) {
        action = (action + 1) % 4;
    }
}

char dir = 'L';
void test_loop()
{
    test_motors();
    /*
    uint8_t l_dir = line_sensors_dir();
    if (l_dir == 2) {
        dir = 'R';
    } else if (l_dir == 6) {
        dir = 'L';
    }
    if (dir == 'L') {
        move_left(230);
    } else if (dir == 'R') {
        move_right(230);
    }
    */
}
# 1 "/home/robotik/Stelly/Stella_X/Stella_X/vic_setup.ino"
void setup_vic_funcs()
{
    /* buttons */
    vic_fn_add("btn", vic_btn_state);
    vic_fn_add("btna", vic_all_btns_state);

    /* leds */
    vic_fn_add("led", vic_led_set);
    vic_fn_add("leda", vic_leds_set_all);

    /* compass */
    vic_fn_add("ca", vic_compass_angle);
    vic_fn_add("cn", vic_compass_north);
    vic_fn_add("csn", vic_compass_set_north);

    /* IR seekers */
    vic_fn_add("IR", vic_read_IR);
    vic_fn_add("IRr", vic_read_IR_raw);
    vic_fn_add("IRra", vic_read_IR_raw_all);
    vic_fn_add("IRa", vic_read_IR_all);
    vic_fn_add("IRd", vic_dir_to_ball);

    /* line sensors */
    vic_fn_add("lS", vic_print_line_sensors);

    /* camera */




    /* dribbler & kicker */
    vic_fn_add("d1", vic_dribbler_on);
    vic_fn_add("d0", vic_dribbler_off);
    vic_fn_add("dk", vic_dribbler_kick);
    vic_fn_add("db", vic_ball_in_dribbler);

    /* motors */
    vic_fn_add("mA", vic_mA);
    vic_fn_add("mB", vic_mB);
    vic_fn_add("mC", vic_mC);

    vic_fn_add("halt", halt);
    vic_fn_add("brake", brake);

    vic_fn_add("u", vic_move_up);
    vic_fn_add("r", vic_move_right);
    vic_fn_add("b", vic_move_back);
    vic_fn_add("l", vic_move_left);
    vic_fn_add("ur", vic_move_up_right);
    vic_fn_add("br", vic_move_back_right);
    vic_fn_add("bl", vic_move_back_left);
    vic_fn_add("ul", vic_move_up_left);

    vic_fn_add("mv", vic_move_to_north);
}

void setup_vic_vars()
{
    vic_var_set("tskid", "0");
    vic_var_bind("tskid", (void *)&task_id, 0x02);
    vic_var_set("lsthresh", "130 /*130*/");
    vic_var_bind("lsthresh", (void *)&light_pwm, 0x02);
    vic_var_set("dT", "0");
    vic_var_bind("dT", (void *)&delta_time, 0x04);
    vic_var_set("uli", "1");
    vic_var_bind("uli", (void *)&line_use_int, 0x02);
    vic_var_set("upixy", "0 /* 1 - goal; 2 - ball */");
    vic_var_bind("upixy", (void *)&use_pixy, 0x02);
}
