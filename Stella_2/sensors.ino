int max_sensor()
{
    uint8_t dir, s1, s2, s3, s4, s5;
    int tsop, tsop2;
  
    int TOO_CLOSE = 320;
    int CLOSE = 390;
    
    int tsop_range = 500; //120
  
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
    if (dir == 0){
        /*
        tsop A8 -> left
        tsop2 A9 -> right
        */
        tsop = analogRead(TSOP_PORT);
        tsop2 = analogRead(TSOP_PORT2);
        
        if (tsop + tsop2 < 2000){
            if ((tsop - tsop2 > 0) && (tsop - tsop2 < tsop_range)){
                sensor = 6;         
            } else if ((tsop2 - tsop > 0) && (tsop2 - tsop < tsop_range)){
                sensor = 3;
            } else {
                sensor = 5;
            } 
            
            
            /*else if (tsop < tsop2){
                if (tsop < TOO_CLOSE){
                    sensor = 5;
                } else if (tsop < CLOSE){
                    sensor = 5;
                } else {
                    sensor = 6;
                }
            } else if (tsop2 < tsop){
                if (tsop2 < TOO_CLOSE){
                    sensor = 5;
                } else if (tsop2 < CLOSE){
                    sensor = 5;
                } else {
                    sensor = 3;
                }
            }*/
        }
    }
  
    return sensor;
}

void sensors_all()
{  
    uint8_t dir, s1, s2, s3, s4, s5;
    seeker.read(&dir, &s1, &s2, &s3, &s4, &s5);
  
    //lavy
    vic_print(analogRead(TSOP_PORT));
    vic_print(" ");
    
    // pravy
    vic_print(analogRead(TSOP_PORT2));
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
