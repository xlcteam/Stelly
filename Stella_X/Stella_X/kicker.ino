void setup_kicker()
{
    pinMode(KICKER_PIN, OUTPUT);
}

void kick() 
{
    digitalWrite(KICKER_PIN, 1);
    delay(KICKER_TIME);
    digitalWrite(KICKER_PIN, 0);
}

void kicker_dribbler()
{
  dribbler_on();
  delay(1000);
  dribbler.off();
  delay(20);
  digitalWrite(KICKER_PIN, 1);
  delay(KICKER_TIME);
  digitalWrite(KICKER_PIN, 0);
  
}
void vic_kick(void)
{
    kick();
    vic_print("kick");
}
