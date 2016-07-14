#define MOTOR_C_PWM_PIN 4
#define MOTOR_C_DIR_PIN 5

void setup()
{
    pinMode(MOTOR_C_PWM_PIN, OUTPUT);
    pinMode(MOTOR_C_DIR_PIN, OUTPUT);
}

void loop()
{
    digitalWrite(MOTOR_C_DIR_PIN, 1);
    analogWrite(MOTOR_C_PWM_PIN, 255);
    delay(1000);

    digitalWrite(MOTOR_C_DIR_PIN, 1);
    analogWrite(MOTOR_C_PWM_PIN, 0);
    delay(1000);

    digitalWrite(MOTOR_C_DIR_PIN, 0);
    analogWrite(MOTOR_C_PWM_PIN, 255);
    delay(1000);
}
