void kick()
{
  digitalWrite(PORT_KICKER, 1);
  delay(kicker_time);
  digitalWrite(PORT_KICKER, 0);
}

void kicking_start()
{
  kicking_running = 1;
  vic_println("Kicking started");
}

void kicking_stop()
{
  kicking_running = 0;
  dribbler_off();
  vic_println("Kicking stopped");
}

void kick_and_drib()
{
  dribbler_off();
  delay(50);
  kick();
  up();
  dribbler.go(-255);
  delay(100);
  dribbler_off();
  stopAllMotors();
}

void kicking ()
{
  dribbler_on();
  delay(2000);
  dribbler_off();
  kick();
  dribbler.go(-255);
  delay(100);
  dribbler_off();
}


