int max_sensor()
{
    uint8_t dir, s1, s2, s3, s4, s5;
    uint16_t tsop, tsop2;
    
  
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
            sensor = 5;
            //sensor = 4;//6
            break;
        case 1: 
            if ((tsop + tsop2) < 2000){
            if (tsop < tsop2) {
            sensor = 6;
            break;
          } else if (tsop2 < tsop) {
              sensor = 3;
              break;
            }
         } else {
             sensor = 5; // sensor = 3;
             break;
         }
        //case 2:
       // case 1:
            //sensor = 3;//5;
    
        case 8:
            sensor = 4; //6;
            break;
        /*case 8:
            sensor = 4;
            break;*/
        case 9:
             if((tsop + tsop2) < 2000) {
              if (tsop < tsop2) {
                sensor = 6;
                break;
              } else if (tsop2 < tsop) {
                 sensor = 3;
                 break;
               }
            } else {
                sensor = 5; // sensor = 6;
                break;
              } 
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
        
        if (tsop + tsop2 < 2000) { //2000
          // vidi lavy
            if (tsop2 < tsop) {//(tsop - tsop2 > 0){
                sensor = 3; 
              //Serial.println("Lavy");  
         // vidi pravy       
            } else if (tsop < tsop2) {//(tsop2 - tsop > 0){
                sensor = 6;
                //Serial.println("Pravy");
            } else {
                sensor = 5;
            }
        }
    }
    
    return sensor;
}

void sensors_all(){  
  while (1 == 1) { 
    uint16_t tsop = analogRead(TSOP_PORT); // pravy
    uint16_t tsop2 = analogRead(TSOP_PORT2); // lavy
    
    uint8_t dir, s1, s2, s3, s4, s5;    
    
    seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
 
  // lavy
  //vic_print(analogRead(TSOP_PORT)); vic_print(" ");
  Serial.print(tsop2); Serial.print(" ");
  
  //pravy
  //vic_print(analogRead(TSOP_PORT2));vic_print(" ");
  Serial.print(tsop); Serial.print(" ");
  
  //Serial.print(tsop3); Serial.print(" ");
  
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
  Serial.print(dir);
  Serial.print(" ");
  
  if ((tsop + tsop2) < 2000) {
    if (tsop < tsop2) {
        Serial.println("Pravy");
    } else if (tsop2 < tsop) { 
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
