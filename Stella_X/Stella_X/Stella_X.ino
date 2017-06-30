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
#define DEBUG(code) do {\
    Serial.println("Debugging:");\
    Serial.println(STR(code));\
    uint32_t _dbg_time_start = micros();\
    code\
    uint32_t _dbg_time_end = micros();\
    Serial.print("Done in ");\
    Serial.print(_dbg_time_end - _dbg_time_start);\
    Serial.println(" microseonds");\
} while(0)

#define LINE_USE_INT 1
#define USE_PIXY 0 /* 1 - goal; 2 - ball */
#define USE_LEGO_COMPASS 1

#define SPEED 180
#define SPEED_NEAR 160
#define PIXY_SPEED 150
#define PIXY_BALL_SPEED 150
#define PIXY_BALL_SPEED_NEAR 130
#define ROTATE_SPEED 70
#define LINE_SPEED 200

#define kP 300
#define kI 300
#define kD 0

#define LINE_BASE_TIME 10
#define LINE_EXTRA_TIME 2000
#define LINE_MAX_DIFF_TIME 10000
#define DRIBBLER_SENSOR_TIME 20
#define LCD_DELAY 100
#define BUTTONS_DELAY 10
#define PIXY_TIME 20

#define LINE_THRESH 130
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

#include "prototypes.h"
/* Arduino puts function prototypes from all ino files "after any comments or
   pre-processor statements (#includes or #defines), but before any other
   statements (including type declarations)."
   So below there is no special function, it actually does nothing, but
   'lets' arduino to generate function prototypes here.
*/
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
uint8_t line_level;
uint8_t compass_right_goal_state;
uint8_t compass_left_goal_state;

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
    if (!mutex[0] && !ws[0] && read_line_sensor(0)) {
        ws[0] = micros();
        halt();
    }

    if (!mutex[1] && !ws[1] && read_line_sensor(1)) {
        ws[1] = micros();
        halt();
    }

    if (!mutex[2] && !ws[2] && read_line_sensor(2)) {
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

    setup_compass();
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
