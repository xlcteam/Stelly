void setup_kicker()
{
    pinMode(KICKER_PIN, OUTPUT);
}

void kick(void) 
{
    analogWrite(KICKER_PIN, HIGH);
    delay(KICKER_TIME);
    analogWrite(KICKER_PIN, LOW);
}

void vic_kick(void)
{
    kick();
}
