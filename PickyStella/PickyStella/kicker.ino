void setup_kicker()
{
    pinMode(KICKER_PIN, OUTPUT);
}

void kick(void) 
{
    digitalWrite(KICKER_PIN, HIGH);
    delay(KICKER_TIME);
    digitalWrite(KICKER_PIN, LOW);
}

void vic_kick(void)
{
    kick();
}
