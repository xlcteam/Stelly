#include <Motor.h>
#include <vic.h>
#include "pinout.h"

#define LINE_THRESH 255
#define LINE_SENSORS_COUNT 6
#define LOG_SIZE 256

uint8_t light_pwm = LINE_THRESH;

uint8_t mutex = 0;
volatile uint8_t line_log[LOG_SIZE];
volatile uint16_t line_log_i = 0;

uint8_t line_sensors[LINE_SENSORS_COUNT] = { LINE_S_FRONT_PIN,
	LINE_S_RIGHT1_PIN, LINE_S_RIGHT2_PIN, LINE_S_BACK_PIN, LINE_S_LEFT1_PIN,
	LINE_S_LEFT2_PIN };
// PB7, PB5, PJ1, PB4, PB6, PJ0

void setup_line_sensors()
{
	for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
		pinMode(line_sensors[i], INPUT);
	}
	pinMode(LINE_S_THRESH_PIN, OUTPUT);
    PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);
    PCMSK1 |= _BV(PCINT9) | _BV(PCINT10);

    line_sensors_update();
}

void line_sensors_update()
{
    analogWrite(LINE_S_THRESH_PIN, light_pwm);
    PCICR |= _BV(PCIE0) | _BV(PCIE1);
}

ISR(PCINT0_vect)
{
    if (mutex || line_log_i >= LOG_SIZE) return;
    line_log[line_log_i++] =
        ((PINB & _BV(PB4)) << 3) |
        ((PINB & _BV(PB5)) << 2) |
        ((PINB & _BV(PB6)) << 1) |
        ((PINB & _BV(PB7)) << 0);
}

ISR(PCINT1_vect)
{
    if (mutex || line_log_i >= LOG_SIZE) return;
    line_log[line_log_i++] = 
        (_BV(7)) |
        ((PINJ & _BV(PJ0)) << 1) |
        ((PINJ & _BV(PJ1)) << 0);
}

void setup()
{
    Serial.begin(115200);
    sei(); /* enables interrupts */
    setup_line_sensors();
}

void loop()
{
    while (!Serial.available());
    while (Serial.available()) {
        Serial.read();
        delay(10);
    }

    mutex = 1;
    Serial.println("##############################");
    for (uint8_t i = 0; i < line_log_i; i++) {
        Serial.println(line_log[i], BIN);
    }
    Serial.println(line_log_i);
    line_log_i = 0;
    mutex = 0;
}
