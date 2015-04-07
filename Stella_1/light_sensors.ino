void line_sensors_all()
{
    int max_val = 1023, max_s = -1, tmp;
  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
        tmp = analogRead(line_sensors[i]);
    
        vic_print(tmp);
        vic_print(" ");
        if(tmp < max_val){
            max_val = tmp;
            max_s = i;
        }
    }
  
    if (max_val > line_min_value) max_s = -1;
  
    vic_print(max_s + 1);
    vic_println();
}


int max_line_sensor()
{
    int max_val = 1023, max_s = -1, tmp;
  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
        tmp = analogRead(line_sensors[i]);

        if(tmp < max_val){
          max_val = tmp;
          max_s = i;
        }
    
        if (max_val < line_min_value){
            return max_s + 1;
        }
    }
  
    if (max_val > line_min_value) max_s = -1;
  
    return max_s + 1;
}

boolean check_light_sensors()
{
    int line_sensor = max_line_sensor();
    if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
       /* switch (line_sensor){
            case 1:
            case 2:
            case 3:
                //if (v_action == 'U')
                    back();
                //else if (v_action == 'B')
                   // up();
                delay(250);
                return true;
        }
    */
        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                    back_right();
                }else if (v_action == 'B'){
                    up_right();
                }
                break;
        
            case 'R':
                if (v_action == 'U'){
                    back_left();
                }else if (v_action == 'B'){
                    up_left();
                }
                break; 
        }
        delay(400);
        stopAllMotors();       
        return true; 
    }
    else {
        return false;
    }
}
