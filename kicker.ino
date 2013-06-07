void kick()
{
  digitalWrite(PORT_KICKER, 1);
  delay(kicker_time);
  digitalWrite(PORT_KICKER, 0);
}


