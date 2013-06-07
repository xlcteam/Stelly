inline void dribbler_off()
{
  dribbler_state = 0;
  digitalWrite(PORT_DRIBBLER, 0);
}

inline void dribbler_on()
{
  dribbler_state = 1;
  digitalWrite(PORT_DRIBBLER, 1);
}


void dribbler_toggle()
{
  if(dribbler_state) {
    dribbler_off();
  } else {
    dribbler_on();
  }
}

int ball_in_dribbler()
{
  return digitalRead(DRIBB_SENSOR_PORT) == 0;
}
