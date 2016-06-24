int max_sensor()
{
    uint8_t dir, s1, s2, s3, s4, s5;
    int tsop, tsop2;
  
    uint8_t tsop_range = 500; // 160,,300,250
  
    int TOO_CLOSE = 400;
    int CLOSE = 500;

    seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
  
    uint8_t sensor = 0;
  
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
            sensor = 4;//6
            break;
    
        //case 2:
        case 1:
            sensor = 3;//5;
    
       // case 8:
        case 9:
            sensor = 6;//4;
            break;
        case 8:
            sensor = 4;
            break;
        case 7:
        //2case 8:
            sensor = 3;
            break;
       
        case 6:
            sensor = 2;
            break;
        
        
    }

    /* direction is zero when the ball is behind the robot, so the IRSeeker doesnt see it */
    if (dir == 0){
      // tsop2 = left
      // tsop = right
        tsop = analogRead(TSOP_PORT); // pravy
        tsop2 = analogRead(TSOP_PORT2); // lavy
         
        if (tsop + tsop2 < 2000){ //2000
          // vidi lavy
            if (tsop - tsop2 > 0){
                sensor = 3; 
              Serial.println("Lavy");  
         // vidi pravy       
            } else if (tsop2 - tsop > 0){
                sensor = 6;
                Serial.println("Pravy");
            } else {
                sensor = 5;
            }
        }
    }
    
    return sensor;
}

void sensors_all(){  
  while (1 == 1) { 
    uint8_t dir, s1, s2, s3, s4, s5;
    seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
   uint8_t tsop = analogRead(TSOP_PORT);
   uint8_t tsop2 = analogRead(TSOP_PORT2);
  // lavy
  Serial.print(tsop2); Serial.print(" ");
  // pravy
  Serial.print(tsop); Serial.print(" ");

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
  Serial.print(" "); 
  
  if ((tsop + tsop2) < 2000) {
    if (tsop < tsop2) {
        Serial.println("Pravy");
    } else (tsop2 < tsop) { 
        Serial.println("Lavy");
      }
  } else {
      Serial.println("NIC");
      }
  }
}


/*void serial_dribbler_sernsor()
{
  vic_func();
  vic_return("%d", ball_in_dribbler());
}*/
