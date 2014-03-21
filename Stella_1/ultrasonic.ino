void us_left_read()
{
  digitalWrite(US_LEFT_W_PIN, HIGH);
  delayMicroseconds(30);
  
  digitalWrite(US_LEFT_W_PIN, LOW);
}

char us_action = ' ';
void us_loop()
{
 /* int range = 7;
  if (!centered(range)) {
    centralize();
  } else {
    stopAllMotors();
  }*/
  us_left_read();
  int value = pulseIn(US_LEFT_R_PIN, HIGH);
  vic_println(value);
  /*if (centered(range)) {
    if (value < 4350){ 
      right();
      us_action = ' ';
    } else if (value > 4720){ 
      left(); 
      us_action = ' ';
    }
    else if (us_action != 'H'){
         stopAllMotors();
         us_action = 'H';
    }
  }*/

}

void ultrasonic_left_start()
{
  ultrasonic_running = 1;
}

void ultrasonic_left_stop()
{
  ultrasonic_running = 0;
}
