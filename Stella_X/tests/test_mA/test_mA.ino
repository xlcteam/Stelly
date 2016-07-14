#define PWM_PIN 3
#define DIR_PIN 4

void setup()
{
    pinMode(DIR_PIN, OUTPUT);
    pinMode(PWM_PIN, OUTPUT);
}

void loop()
{
    digitalWrite(DIR_PIN, HIGH);
    analogWrite(PWM_PIN, 200);
}
