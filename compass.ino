
void serial_compass()
{
  /*
  vicore_func();
  vicore_return("%f", getCompass());
  */
  Serial.println(compass.angle());
}

void compass_load()
{
  compass.set_north();
}

void compass_default()
{
  Serial.println(compass.real_north());
}

void serial_compass_loop_start()
{
  compass_loop = 1;
}

void serial_compass_loop_stop()
{
  compass_loop = 0;
}


boolean centered(int range)
{
  float c = compass.angle();
  return ((c >= (360.0 - range)) || (c <= range));
}

void centralize() 
{
  int c = (int) compass.angle();
  check_light_sensors();
  if (c > 180) {
    c = 360 - c;
    rrotate(speed_rotate/2 + (c*speed_rotate/180));
  } else
    lrotate(speed_rotate/2 + (c*speed_rotate/180));
}



