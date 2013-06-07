 
void mA(void)
{
  int s;
  vic_args("%d", &s);
  motorA.go(s);
}

void mB(void)
{
  int s;
  vic_args("%d", &s);
  motorB.go(s);  
}

void mC(void)
{
  int s;
  vic_args("%d", &s);
  motorC.go(s);  
}

void motion_start()
{
  motion_running = 1;
  vic_println("AutoMotion started");
  
}

void motion_stop()
{
  motion_running = 0;
  stopAllMotors();
  dribbler_off();
  vic_println("AutoMotion stopped");
}

void stopAllMotors()
{
  motorA.stop();
  motorB.stop();
  motorC.stop();
}


void motion()
{
  
  int range = 7;
  if (!centered(range)) {
    centralize();
    //Serial.println("centralizing!");
    return;
  } else {
    stopAllMotors();
  }
  /*
  int line_sensor = max_line_sensor();
  if (line_sensor != 0) {
    switch (action) {
      case 'L':
        right();
        break;
        
      case 'R':
        left();
        break;    
        
      case 'U':
        back();
        break;
      
      case 'B':
        up();
        break;
    }
    Serial.println("from white");
    delay(500);
    return;
  }
  */

  
  int sensor = max_sensor();
  
  // fixating sensor 2 and 7 so that sensor 1 doesn't see everything
  if (sensor == 1 && analogRead(sensors[6]) < LEFT_NEAR)
    sensor = 7;

  if (sensor == 1 && analogRead(sensors[1]) < RIGHT_NEAR)
    sensor = 2;
  
  switch (sensor) {
    case 2:
      right();
      dribbler_on();
      break; 
      
     case 7:
      dribbler_on();
      left();
      break;
    
    
    case 3:
      back_right();
      break;
    
    case 6:
      back_left();
      break;
      
    case 5: 
    case 4:
      back();
      break;
    
    case 1:
      dribbler_on();
      
      if (ball_in_dribbler() && (analogRead(sensors[0]) < 310) ) {
        stopAllMotors();
        delay(200);
        if (ball_in_dribbler()) {
          kick();
        }
        
        return;
      }
      
      up();
      break;
      
      
    default:
      dribbler_off();
      stopAllMotors();
      break;
  }

}

void is_ball_near()
{
  int sensor = max_sensor();
  
  if (analogRead(sensors[6]) < LEFT_NEAR && sensor == 1){
    Serial3.println("left");
  } else if (analogRead(sensors[1]) < RIGHT_NEAR && sensor == 1) {
    Serial3.println("right");
  } else if (sensor == 1){
    Serial3.println("forward");
  } else {
    Serial3.println("0");
  }
}

void is_ball_near_right()
{
  // near second sensor
  int sensor = max_sensor();
  
  if (analogRead(sensors[1]) < RIGHT_NEAR && sensor == 1){
    Serial3.println("1");
  } else {
    Serial3.println("0");
  }

}

void right()
{
  action = 'R';
  
  motorA.go(speed/2);
  motorB.go(speed);
  motorC.go(-speed);
}

void left()
{
  action = 'L';
  
  motorA.go(-speed);
  motorB.go(-speed/2);
  motorC.go(speed);
}

void up()
{
  action = 'U';
  
  motorA.go(-speed);
  motorB.go(speed);
  motorC.stop();
}

void back()
{
  action = 'B';
  
  motorA.go(speed);
  motorB.go(-speed);
  motorC.stop();
}


void halt()
{
  motorA.stop();
  motorB.stop();
  motorC.stop();
}

void back_right()
{
  action = 'B';
  
  motorA.go(speed);
  motorB.stop();
  motorC.go(-speed); 
}

void back_left()
{
  action = 'B';
  
  motorA.stop();
  motorB.go(-speed);
  motorC.go(speed);  
}

void rrotate(int spd)
{
  motorA.go(spd);
  motorB.go(-spd);
  motorC.go(spd);
}

void lrotate(int spd)
{
  motorA.go(-spd);
  motorB.go(spd);
  motorC.go(-spd);  
}


void special_movement_up() 
{
  int c = (int) compass.angle();
  int diff = speed - speed_min;
  
  int range = 40;
  
  if (c > 180) {
    c = 360 - c;
    
    if (c > range) {
      rrotate(speed_rotate);
      return;
    }
    
    motorA.go(speed);
    motorB.go(speed - (diff/range)*c);
    motorC.go(speed);
    
    Serial3.print("- left side ");
    Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(speed - (diff/range)*c);
    motorB.go(speed);
    motorC.go(speed - (diff/range)*c);
    
    Serial3.print("- right side ");
    Serial3.println(c);    
  } 
}


void special_movement_back() 
{
  int c = (int) compass.angle();
  int diff = speed - speed_min;
  
  int range = 40;
  
  if (c > 180) {
    c = 360 - c;
    
    if (c > range) {
      rrotate(speed_rotate);
      return;
    }
    
    motorA.go(-(speed - (diff/range)*c));
    motorB.go(-speed);
    motorC.go(-(speed - (diff/range)*c));
    
    Serial3.print("- left side ");
    Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(-speed);
    motorB.go(-(speed - (diff/range)*c));
    motorC.go(-speed);
    
    Serial3.print("- right side ");
    Serial3.println(c);    
  } 
}


void special_movement_left() 
{
  int c = (int) compass.angle();
  int diff = speed - speed_min;
  
  int range = 40;
  
  if (c > 180) {
    c = 360 - c;
    
    if (c > range) {
      rrotate(speed_rotate);
      return;
    }
    
    motorA.go(-(speed - (diff/range)*c));
    motorB.go((speed - (diff/range)*c));
    motorC.go(speed);
    
    Serial3.print("- up side ");
    Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(-speed);
    motorB.go(speed);
    motorC.go((speed - (diff/range)*c));
    
    Serial3.print("- back side ");
    Serial3.println(c);    
  } 
}

void special_movement_right() 
{
  int c = (int) compass.angle();
  int diff = speed - speed_min;
  
  int range = 40;
  
  if (c > 180) {
    c = 360 - c;
    
    if (c > range) {
      rrotate(speed_rotate);
      return;
    }
    
    motorA.go((speed - (diff/range)*c));
    motorB.go(-(speed - (diff/range)*c));
    motorC.go(-speed);
    
    Serial3.print("- up side ");
    Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(speed);
    motorB.go(-speed);
    motorC.go(-(speed - (diff/range)*c));
    
    Serial3.print("- back side ");
    Serial3.println(c);    
  } 
}
