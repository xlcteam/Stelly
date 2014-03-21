int max_sensor()
{
  uint8_t dir, s1, s2, s3, s4, s5;
  seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
  
  int sensor = 0;
  
  switch (dir) {
    case 5:
      sensor = 1;
      break;
    
    case 4: 
      sensor = 7;
      break;
    
    case 3:
      sensor = 6;
      break;
    
    case 2:
    case 1:
      sensor = 5;
    
    case 8:
    case 9:
      sensor = 4;
      break;
    
    case 7:
      sensor = 3;
      break;
    
    case 6:
      sensor = 2;
      break;
  }
  
  // This means that the sensor is directly behind the robot.
  // A dangerous situation, setting sensor to 6 means the robot will move to some side 
  if (analogRead(A0) < REAR_NEAR) {
    sensor = 6;
  
  }
  
  
  return sensor;
}

void sensors_all()
{  
  uint8_t dir, s1, s2, s3, s4, s5;
  seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);

  vic_print(analogRead(A0));
  vic_print(" ");
  
  vic_print(s1);
  vic_print(" ");
  vic_print(s2);
  vic_print(" ");
  vic_print(s3);
  vic_print(" ");
  vic_print(s4);
  vic_print(" ");
  vic_print(s5);
  vic_print(" ");  
  vic_print(dir);
  vic_print(" ");  
  vic_print(max_sensor());  
  vic_println();
  
}


void serial_dribbler_sernsor()
{
  vic_func();
  vic_return("%d", ball_in_dribbler());
}



