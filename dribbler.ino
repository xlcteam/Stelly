inline void dribbler_off()
{
  dribbler_state = 0;
  dribbler.stop();
}

inline void dribbler_on()
{
  dribbler_state = 1;
  dribbler.go(255);
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
