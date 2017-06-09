#include <Motor.h>
#include <vic.h>
#include "pinout.h"

#define LS_front() ((PINB >> PB5) & 0x01)
#define LS_right() ((PINB >> PB7) & 0x01)
#define LS_left() ((PINB >> PB4) & 0x01)

#define LINE_THRESH 130
#define LINE_SENSORS_COUNT 3
#define LOG_SIZE 256

uint8_t light_pwm = LINE_THRESH;

uint8_t mutex = 0;
volatile uint32_t line_log[LOG_SIZE];
volatile uint16_t line_log_i = 0;

uint8_t line_sensors[LINE_SENSORS_COUNT] =
    { LINE_S1_PIN, LINE_S2_PIN, LINE_S4_PIN };

void setup_line_sensors()
{
    for (uint8_t i = 0; i < LINE_SENSORS_COUNT; i++) {
        pinMode(line_sensors[i], INPUT);
    }
    pinMode(LINE_THRESH_PIN, OUTPUT);

    PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT7);

    line_sensors_update();
}

void line_sensors_update()
{
    analogWrite(LINE_THRESH_PIN, light_pwm);
    PCICR |= _BV(PCIE0); // set
}

ISR(PCINT0_vect)
{
    if (!mutex && line_log_i < LOG_SIZE) {
        line_log[line_log_i++] = (
        ( ((uint32_t)LS_front() << 2) |
          ((uint32_t)LS_right() << 1) |
           (uint32_t)LS_left() ) << 29) | (uint32_t)micros();
    }
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
        //Serial.print(line_log[i], BIN);
        //Serial.print(" ");
        Serial.print((line_log[i] << 0) >> 31);
        Serial.print(" ");
        Serial.print((line_log[i] << 1) >> 31);
        Serial.print(" ");
        Serial.print((line_log[i] << 2) >> 31);
        Serial.print(" ");
        Serial.println((line_log[i] << 3) >> 3);
    }
    Serial.println(micros());
    Serial.println(line_log_i);
    line_log_i = 0;
    mutex = 0;
}
