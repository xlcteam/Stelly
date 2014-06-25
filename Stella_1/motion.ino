 
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
  
  if (check_light_sensors()) return;
  
  if (!centered(range)) {
    centralize();
    //Serial.println("centralizing!");
    return;
  } else {
    stopAllMotors();
    
    //up();
  }
    
  int sensor = max_sensor();
  
  dribbler_off();
  switch (sensor) {
    case 2:
      //right();
      up_right();
      dribbler_on();
      break; 
      
     case 7:
      //left();
      up_left();
      dribbler_on();
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
      /*if (ball_in_dribbler()) {
        stopAllMotors();
        delay(500);
        if (ball_in_dribbler()) {
          //kick();
          kick_and_drib();
        }
      }*/
      
      up();
      break;
      
      
    default:
      dribbler_off();
      stopAllMotors();
      break;
  }
  //Serial.println(action);
/**/
}

void right()
{
  h_action = 'R';
  
  motorA.go(-speed/2);
  motorB.go(-speed/2);
  motorC.go(speed);  
}

void left()
{
  h_action = 'L';
  
  motorA.go(speed/2);
  motorB.go(speed/2);
  motorC.go(-speed);  
}

void up()
{
  v_action = 'U';
  
  motorA.go(-speed);
  motorB.go(speed);
  motorC.stop();
}

void back()
{
  v_action = 'B';

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

void up_right()
{
  h_action = 'L';
  v_action = 'U';
  
  motorA.go(-speed);
  motorB.stop();
  motorC.go(speed);
}

void up_left()
{
  h_action = 'R';
  v_action = 'U';
  
  motorA.stop();
  motorB.go(speed);
  motorC.go(-speed);
}

void back_right()
{
  h_action = 'R';
  v_action = 'B';
  
  motorA.stop();
  motorB.go(-speed);
  motorC.go(speed);   
}

void back_left()
{
  h_action = 'L';
  v_action = 'B';
  
  motorA.go(speed);
  motorB.stop();
  motorC.go(-speed);   
}

void rrotate(int spd)
{
  motorA.go(-spd);
  motorB.go(-spd);
  motorC.go(-spd);
}

void lrotate(int spd)
{
  motorA.go(spd);
  motorB.go(spd);
  motorC.go(spd);  
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
    
    //Serial3.print("- left side ");
    //Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(speed - (diff/range)*c);
    motorB.go(speed);
    motorC.go(speed - (diff/range)*c);
    
    //Serial3.print("- right side ");
    //Serial3.println(c);    
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
    
   //Serial3.print("- left side ");
    //Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(-speed);
    motorB.go(-(speed - (diff/range)*c));
    motorC.go(-speed);
    
    //Serial3.print("- right side ");
   // Serial3.println(c);    
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
    
    //Serial3.print("- up side ");
    //Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(-speed);
    motorB.go(speed);
    motorC.go((speed - (diff/range)*c));
    //
    //Serial3.print("- back side ");
    //Serial3.println(c);    
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
    
    //Serial3.print("- up side ");
    //Serial3.println(c);
    
  } else {
    
    if (c > range) {
      lrotate(speed_rotate);
      return;
    }    
    
    motorA.go(speed);
    motorB.go(-speed);
    motorC.go(-(speed - (diff/range)*c));
    
   // Serial3.print("- back side ");
   // Serial3.println(c);    
  } 
}
