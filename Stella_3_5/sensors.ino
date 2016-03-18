int max_sensor()
{
    uint8_t dir, s1, s2, s3, s4, s5;
    int tsop, tsop2;
  
    int tsop_range = 450; // 160,,300,250,500
  
    int TOO_CLOSE = 400;
    int CLOSE = 500;

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

    /* direction is zero when the ball is behind the robot, so the IRSeeker doesnt see it */
   /* if (dir == 0){
      // tsop2 = left = A6
      // tsop = right = A7
        tsop = analogRead(TSOP_PORT); // pravy
        tsop2 = analogRead(TSOP_PORT2); // lavy
         
        if (tsop + tsop2 < 2000){
          // vidi lavy => tsop2
            if ((tsop - tsop2 > 0) && (tsop - tsop2 < tsop_range)){
                sensor = 3;   
         // vidi pravy => tsop      
            } else if ((tsop2 - tsop > 0) && (tsop2 - tsop < tsop_range)){
                sensor = 6;
            } else {
                sensor = 5;
            }
        }
    }*/
    
    return sensor;
}

void sensors_all()
{  
  uint8_t dir, s1, s2, s3, s4, s5;
  seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
 
  // pravy
 //vic_print(analogRead(TSOP_PORT));vic_print(" ");
  Serial.print(analogRead(TSOP_PORT)); Serial.print(" ");
  
  //lavy
 //vic_print(analogRead(TSOP_PORT2));vic_print(" ");
  Serial.print(analogRead(TSOP_PORT2)); Serial.print(" ");
  
  /*vic_print(dir);
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
  //vic_print(" ");    
  //vic_print(max_sensor());  
  vic_println();*/
  Serial.print(dir);
  Serial.print(" ");
  Serial.print(s1);
  Serial.print(" ");
  Serial.print(s2);
  Serial.print(" ");
  Serial.print(s3);
  Serial.print(" ");
  Serial.print(s4);
  Serial.print(" ");
  Serial.print(s5);
  Serial.println();
}

/*void serial_dribbler_sernsor()
{
  vic_func();
  vic_return("%d", ball_in_dribbler());
}*/
