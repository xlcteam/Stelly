#include <Arduino.h>
#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
#include <LiquidCrystal.h>
#include <Motor.h>
#include <Wire.h>
#include <RelativeHMC6352.h>
#include <SPI.h>
#include <Pixy.h>
#include <PixyViSy.h>
#include <vic.h>
#include <Buttons.h>
#include "pinout.h"

#define STR(str) #str
#define XSTR(str) STR(str)

#define LINE_USE_INT 1
#define USE_PIXY 0 /* 1 - goal; 2 - ball */

#define SPEED 180 // 180
#define SPEED_NEAR 160 // 160
#define PIXY_SPEED 150
#define PIXY_BALL_SPEED 150
#define PIXY_BALL_SPEED_NEAR 130
#define ROTATE_SPEED 70
#define LINE_SPEED 200

#define LINE_TIME 10
#define LINE_MAX_DIFF_TIME 10000
#define LINE_D_TIME_MIN 100
#define LINE_TIME_MIN 400
#define DRIBBLER_SENSOR_TIME 20
#define LCD_DELAY 100
#define BUTTONS_DELAY 10
#define PIXY_TIME 20

#define LINE_THRESH 130 //130
#define TO_NORTH_THRESH 30
#define IR_FAR_THRESH 550
#define BALL_FAR_THRESH 400 /* for orange ball */
#define IR_NO_BALL 1000

#define IR_SENSORS_COUNT 14
#define BUTTONS_COUNT 4
#define LEDS_COUNT 4
#define LINE_SENSORS_COUNT 3
#define SCREENS_COUNT 4

#define PIXY_FOV 70
#define IR_BALL_SIZE 75
#define ORANGE_BALL_SIZE 67

#define WS_SAFE(f) do { if (!(ws[0] || ws[1] || ws[2])) {f;} } while (0)

#define TASKS_COUNT 4
#define TASK_NO 0
#define TASK_DISPLAY 1
#define TASK_TEST 2
#define TASK_MOTION 3
#define START_TASK TASK_NO

/* Arduino puts function prototypes from all ino files "after any comments or
   pre-processor statements (#includes or #defines), but before any other
   statements (including type declarations)."
   So below there is no special function, it actually does nothing, but
   'lets' arduino to generate function prototypes here.
*/
#line 66 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void Arduino_IDE_will_generate_function_prototypes_here();
#line 89 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void vic_camera_data(void);
#line 98 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void IDLE(void);
#line 127 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void serial_output(char c);
#line 133 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void setup();
#line 155 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void loop();
#line 11 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void setup_IR_sensors();
#line 18 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint16_t read_IR_raw(uint8_t index);
#line 23 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint16_t read_IR(uint8_t i);
#line 30 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void read_IR_all(uint16_t *out_data);
#line 37 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void read_IR_raw_all(uint16_t *out_data);
#line 44 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint8_t max_IR();
#line 62 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint16_t dir_to_ball();
#line 104 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void vic_read_IR(void);
#line 112 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void vic_read_IR_raw(void);
#line 120 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void vic_read_IR_all(void);
#line 130 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void vic_read_IR_raw_all(void);
#line 140 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
void vic_dir_to_ball(void);
#line 4 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
uint8_t btn_state(uint8_t index);
#line 9 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void setup_buttons();
#line 22 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void update_buttons();
#line 35 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void vic_btn_state(void);
#line 43 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void vic_all_btns_state(void);
#line 58 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn1_onPush(void);
#line 63 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn1_onRelease(void);
#line 94 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn2_onPush(void);
#line 114 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn3_onPush(void);
#line 134 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn4_pushed(void);
#line 141 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
void btn4_onPush(void);
#line 3 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
uint16_t compass_angle(void);
#line 8 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
uint16_t compass_angle_raw(void);
#line 13 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
uint16_t compass_north(void);
#line 18 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
void compass_set_north_val(uint16_t value);
#line 23 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
void compass_set_north(void);
#line 28 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
bool centered(uint16_t range);
#line 36 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
void vic_compass_angle(void);
#line 41 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
void vic_compass_north(void);
#line 46 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
void vic_compass_set_north(void);
#line 4 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void setup_display();
#line 10 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_idle();
#line 19 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_1(void);
#line 49 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_2(void);
#line 76 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_3(void);
#line 103 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_4(void);
#line 119 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
void print_display_screen(void);
#line 4 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void setup_dribbler();
#line 11 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void dribbler_off();
#line 17 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void dribbler_on();
#line 23 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void dribbler_switch();
#line 29 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void dribbler_switch_wk();
#line 39 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void dribbler_kick();
#line 45 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
bool ball_in_dribbler();
#line 60 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void vic_dribbler_off(void);
#line 65 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void vic_dribbler_on(void);
#line 70 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void vic_dribbler_kick(void);
#line 75 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
void vic_ball_in_dribbler(void);
#line 3 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
void setup_leds();
#line 10 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
void led_set(uint8_t index, uint8_t state);
#line 15 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
void leds_set_all(uint8_t states);
#line 24 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
void vic_led_set(void);
#line 32 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
void vic_leds_set_all(void);
#line 4 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
void setup_line_sensors();
#line 16 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
uint8_t read_line_sensor(uint8_t index);
#line 21 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
void line_sensors_update();
#line 35 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
uint8_t process_ws();
#line 131 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
uint8_t line_sensors_dir();
#line 236 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
uint8_t line_for_display();
#line 247 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
void vic_print_line_sensors(void);
#line 8 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
char motion_get_ball_side();
#line 10 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void motion_start();
#line 31 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void motion();
#line 76 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void motion_line(uint8_t dir);
#line 133 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void motion_ball(uint16_t dir);
#line 322 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void pixy_motion_ball();
#line 445 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
void pixy_motion_goal();
#line 5 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void brake(void);
#line 12 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void halt(void);
#line 19 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
int16_t spd_to_north(int16_t speeds[3]);
#line 44 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_raw(int16_t speeds[3]);
#line 51 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_to_north(int16_t speeds[3]);
#line 59 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_up(uint8_t spd);
#line 65 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_right(uint8_t spd);
#line 71 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_back(uint8_t spd);
#line 77 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_left(uint8_t spd);
#line 83 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_up_right(uint8_t spd);
#line 89 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_back_right(uint8_t spd);
#line 95 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_back_left(uint8_t spd);
#line 101 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void move_up_left(uint8_t spd);
#line 107 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void rotate(int16_t spd);
#line 114 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void centralize();
#line 122 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_to_north(void);
#line 133 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_mA(void);
#line 134 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_mB(void);
#line 135 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_mC(void);
#line 137 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_up(void);
#line 147 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_right(void);
#line 157 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_back(void);
#line 167 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_left(void);
#line 177 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_up_right(void);
#line 187 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_back_right(void);
#line 197 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_back_left(void);
#line 207 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
void vic_move_up_left(void);
#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/test.ino"
void test_start();
#line 10 "/home/robotik/Stelly/Stella_X/Stella_X/test.ino"
void test_motors();
#line 28 "/home/robotik/Stelly/Stella_X/Stella_X/test.ino"
void test_loop();
#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/vic_setup.ino"
void setup_vic_funcs();
#line 57 "/home/robotik/Stelly/Stella_X/Stella_X/vic_setup.ino"
void setup_vic_vars();
#line 66 "/home/robotik/Stelly/Stella_X/Stella_X/Stella_X.ino"
void Arduino_IDE_will_generate_function_prototypes_here() {return;}

uint8_t line_use_int = LINE_USE_INT;
uint8_t use_pixy = USE_PIXY;
uint8_t light_pwm = LINE_THRESH;
uint8_t actual_screen;
uint8_t motion_dbg = 0;
uint8_t test_dbg = 0;
uint32_t last_time;
uint16_t delta_time;
uint8_t motion_last_dir;
uint8_t line_speed_down;

PixyViSy pixyViSy(1, 120, IR_BALL_SIZE);

void (*tasks[TASKS_COUNT])(void) = {
    NULL,
    print_display_screen,
    test_loop,
    motion
};
uint8_t task_id = START_TASK;

void vic_camera_data(void)
{
    if (use_pixy == 1) {
        vic_printf("%c %u\n", pixyViSy.getAction(), pixyViSy.getDistance());
    } else if (use_pixy == 2) {
        vic_printf("%i %u\n", pixyViSy.getBallAngle(), pixyViSy.getDistance());
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

ISR(PCINT0_vect)
{
    if (!mutex[0] && PINB & _BV(PB5) && !ws[0]) {
        ws[0] = micros();
        halt();
    }

    if (!mutex[1] && PINB & _BV(PB7) && !ws[1]) {
        ws[1] = micros();
        halt();
    }

    if (!mutex[2] && PINB & _BV(PB4) && !ws[2]) {
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
    sei(); /* enables interrupts */

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

    if (tasks[task_id] != NULL) {
        tasks[task_id]();
    }

    line_sensors_update();

    led_set(3, use_pixy);
    led_set(1, use_pixy == 2);
    pixyViSy.setBallSize(use_pixy == 2 ? ORANGE_BALL_SIZE : IR_BALL_SIZE);

    delta_time = millis() - last_time;
    last_time = millis();
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/IR_sensors.ino"
uint8_t IR_sensors_pins[IR_SENSORS_COUNT] = { IR_BOTTOM2_PIN, IR1_PIN, IR2_PIN,
    IR3_PIN, IR4_PIN, IR5_PIN, IR6_PIN, IR7_PIN, IR8_PIN, IR9_PIN, IR10_PIN,
    IR11_PIN, IR_BOTTOM3_PIN, IR_BOTTOM1_PIN };
uint16_t IR_vals_far[IR_SENSORS_COUNT] =
    { 813, 1008, 1009, 1011, 1014, 1015, 1017,
      1016, 1016, 1015, 1012, 1015, 1015, 1013 };
uint16_t IR_vals_near[IR_SENSORS_COUNT] =
    { 116, 114, 115, 117, 117, 116, 117,
      115, 109, 111, 113, 117, 140, 163 };

void setup_IR_sensors()
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        pinMode(IR_sensors_pins[i], INPUT);
    }
}

inline uint16_t read_IR_raw(uint8_t index)
{
    return analogRead(IR_sensors_pins[index]);
}

uint16_t read_IR(uint8_t i)
{
    uint16_t val = read_IR_raw(i);
    val = constrain(val, IR_vals_near[i], IR_vals_far[i]);
    return map(val, IR_vals_near[i], IR_vals_far[i], 0, 1024);
}

void read_IR_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        out_data[i] = read_IR(i);
    }
}

void read_IR_raw_all(uint16_t *out_data)
{
    for (uint8_t i = 0; i < IR_SENSORS_COUNT; i++) {
        out_data[i] = read_IR_raw(i);
    }
}

uint8_t max_IR()
{
    uint16_t data[IR_SENSORS_COUNT];
    uint16_t max_index = 0;

    read_IR_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 2; i++) { // last 2 are special
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
    uint16_t data[IR_SENSORS_COUNT];
    uint8_t nearest = 0;
    read_IR_all(data);

    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 2; i++) {
        if (data[i] < data[nearest]) {
            nearest = i;
        }
    }

    if (data[nearest] > IR_NO_BALL) {
        return ~0;
    }

    if (ball_in_dribbler() && data[0] < IR_NO_BALL) {
        return 0;
    }

    if (data[nearest] > IR_FAR_THRESH) {
        return 1000 + nearest * 30;
    }

    if (nearest == 1) {
        if (data[12] > IR_FAR_THRESH) {
            return 30;
        } else {
            return 15;
        }
    } else if (nearest == 11) {
        if (data[13] > IR_FAR_THRESH) {
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
    if (vic_args("%hhu", &index) == 1) {
        vic_printf("%u\n", read_IR(index));
    }
}

void vic_read_IR_raw(void)
{
    uint8_t index;
    if (vic_args("%hhu", &index) == 1) {
        vic_printf("%u\n", read_IR_raw(index));
    }
}

void vic_read_IR_all(void)
{
    uint16_t data[IR_SENSORS_COUNT];
    read_IR_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 1; i++) {
        vic_printf("%4u ", data[i]);
    }
    vic_printf("%4u\n", data[IR_SENSORS_COUNT - 1]);
}

void vic_read_IR_raw_all(void)
{
    uint16_t data[IR_SENSORS_COUNT];
    read_IR_raw_all(data);
    for (uint8_t i = 0; i < IR_SENSORS_COUNT - 1; i++) {
        vic_printf("%4u ", data[i]);
    }
    vic_printf("%4u\n", data[IR_SENSORS_COUNT - 1]);
}

void vic_dir_to_ball(void)
{
    uint16_t dir = dir_to_ball();
    if (dir == ~(uint16_t)0) {
        vic_println("no ball");
    } else if (dir > 1000) {
        vic_printf("%u far\n", dir - 1000);
    } else {
        vic_printf("%u near\n", dir);
    }
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/buttons.ino"
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

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/compass.ino"
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
    vic_printf("%u\n", compass_angle());
}

inline void vic_compass_north(void)
{
    vic_printf("%u\n", (uint16_t) compass_north());
}

inline void vic_compass_set_north(void)
{
    compass_set_north();
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/display.ino"
LiquidCrystal lcd(
    LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void setup_display()
{
    lcd.begin(16, 2);
    actual_screen = 0;
}

void print_display_idle()
{
    delay(LCD_DELAY);
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
    lcd.print(min(read_IR(0), 999));
    lcd.setCursor(9, 0);
    lcd.print(min(read_IR(1), 999));
    lcd.setCursor(13, 0);
    lcd.print(min(read_IR(2), 999));
    lcd.setCursor(0, 1);
    lcd.print(min(read_IR(3), 999));
    lcd.setCursor(4, 1);
    lcd.print(min(read_IR(4), 999));
    lcd.setCursor(8, 1);
    lcd.print(min(read_IR(5), 999));
    lcd.setCursor(12, 1);
    lcd.print(min(read_IR(6), 999));
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
    lcd.print(min(read_IR(7), 999));
    lcd.setCursor(9, 0);
    lcd.print(min(read_IR(8), 999));
    lcd.setCursor(13, 0);
    lcd.print(min(read_IR(9), 999));
    lcd.setCursor(0, 1);
    lcd.print(min(read_IR(10), 999));
    lcd.setCursor(4, 1);
    lcd.print(min(read_IR(11), 999));
    lcd.setCursor(8, 1);
    lcd.print(min(read_IR(12), 999));
    lcd.setCursor(12, 1);
    lcd.print(min(read_IR(13), 999));
}

void print_display_4(void)
{
    lcd.print("WL  1 2 3");
    lcd.setCursor(0, 1);
    lcd.print(light_pwm);
    uint8_t i;
    for (i = 0; i < LINE_SENSORS_COUNT; i++) {
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
    if (millis() - time > LCD_DELAY) {
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

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/dribbler.ino"
Motor dribbler = Motor(MOTOR_C_DIR_PIN, MOTOR_C_PWM_PIN);
int8_t dribbler_state;

void setup_dribbler()
{
    pinMode(DRIBBLER_PIN, OUTPUT);
    pinMode(DRIBBLER_SENSOR_PIN, INPUT);
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
    if (!digitalRead(DRIBBLER_SENSOR_PIN)) {
        l_time = millis();
        return true;
    }
    if (millis() - l_time < DRIBBLER_SENSOR_TIME) {
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
    vic_printf("%u\n", (uint8_t) ball_in_dribbler());
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/leds.ino"
uint8_t leds[LEDS_COUNT] = { LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN };

void setup_leds()
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++) {
        pinMode(leds[i], OUTPUT);
    }
}

inline void led_set(uint8_t index, uint8_t state)
{
    digitalWrite(leds[index], state);
}

void leds_set_all(uint8_t states)
{
    for (uint8_t i = 0; i < LEDS_COUNT; i++) {
        digitalWrite(leds[i], states & (0x01 << i));
    }
}

/* VIC FUNCTIONS */

void vic_led_set(void)
{
    uint8_t index, state;
    if (vic_args("%hhu %hhu", &index, &state) == 2) {
        led_set(index, state);
    }
}

void vic_leds_set_all(void)
{
    uint8_t states;
    if (vic_args("%hhu", &states) == 1) {
        leds_set_all(states);
    }
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/line_sensors.ino"
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
        if (motion_last_dir < 180 && motion_last_dir > 15) {
            dir = 5;
        } else if (motion_last_dir > 180 && motion_last_dir < 345) {
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

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/motion.ino"
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
        for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++){
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
                } else if (use_pixy == 1) {
                    pixy_motion_goal();
                } else {
                    WS_SAFE(move_up(speed); motion_last_dir = 0);
                    dribbler_kick();
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

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/movements.ino"
Motor motorA = Motor(MOTOR_A_DIR_PIN, MOTOR_A_PWM_PIN);
Motor motorB = Motor(MOTOR_B_DIR_PIN, MOTOR_B_PWM_PIN);
Motor motorC = Motor(MOTOR_D_DIR_PIN, MOTOR_D_PWM_PIN);

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

    max_speed = abs(speeds[0]);
    if (abs(speeds[1]) > max_speed) { max_speed = abs(speeds[1]); }
    if (abs(speeds[2]) > max_speed) { max_speed = abs(speeds[2]); }

    max_diff = 255 - max_speed;

    if (angle > TO_NORTH_THRESH) {
        compensation = ROTATE_SPEED;
    } else if (angle < -TO_NORTH_THRESH) {
        compensation = -ROTATE_SPEED;
    } else {
        compensation = ROTATE_SPEED * angle / TO_NORTH_THRESH;
        compensation = compensation * abs(angle) / TO_NORTH_THRESH;
    }

    return constrain(compensation, -max_diff, max_diff);
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

    if (vic_args("%d %d %d", &spds[0], &spds[1], &spds[2]) == 3) {
        move_to_north(spds);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_mA(void) { int16_t s; vic_args("%d", &s); motorA.go(s); }
void vic_mB(void) { int16_t s; vic_args("%d", &s); motorB.go(s); }
void vic_mC(void) { int16_t s; vic_args("%d", &s); motorC.go(s); }

void vic_move_up(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_up_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back_right(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back_right(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_back_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_back_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

void vic_move_up_left(void)
{
    uint8_t spd;
    if (vic_args("%hhu", &spd) == 1) {
        move_up_left(spd);
    } else {
        vic_print_err(VIC_ERR_INVALID_ARGS);
    }
}

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/pixy_motion_ball.ino"

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/test.ino"
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
        move_up(SPEED);
    else if (action == 2)
        move_back(SPEED);
    else if (action == 3)
        move_left(SPEED);
    else
        move_right(SPEED);
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

#line 1 "/home/robotik/Stelly/Stella_X/Stella_X/vic_setup.ino"
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
#if USE_PIXY == 1
    vic_fn_add("cam", vic_camera_data);
#endif

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
    vic_var_set("tskid", XSTR(START_TASK));
    vic_var_bind("tskid", (void *)&task_id, VIC_VAR_UINT8);
    vic_var_set("lsthresh", XSTR(LINE_THRESH));
    vic_var_bind("lsthresh", (void *)&light_pwm, VIC_VAR_UINT8);
    vic_var_set("dT", "0");
    vic_var_bind("dT", (void *)&delta_time, VIC_VAR_UINT16);
    vic_var_set("uli", XSTR(LINE_USE_INT));
    vic_var_bind("uli", (void *)&line_use_int, VIC_VAR_UINT8);
    vic_var_set("upixy", XSTR(USE_PIXY));
    vic_var_bind("upixy", (void *)&use_pixy, VIC_VAR_UINT8);
}

