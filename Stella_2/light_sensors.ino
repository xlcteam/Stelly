void line_sensors_all()
{/*
    int max_val = 1023, max_s = -1, tmp;
  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
        tmp = analogRead(line_sensors[i]);

        //vic_print(tmp);
        //vic_print(" ");
        Serial.print(tmp);
        Serial.print(" ");
        if(tmp < max_val){
            max_val = tmp;
            max_s = i;

            if (max_val > line_values[max_s]) max_s = -1;
        }
    }

    //vic_print(max_s + 1);
    //vic_println();
    Serial.print(max_s + 1);
    Serial.println();*/
    while (1 == 1){
    for (int i = 0; i < sizeof(line_sensors)/sizeof(*line_sensors); i++) {
        Serial.print(digitalRead(line_sensors[i]));
        Serial.print(" ");
     }
    Serial.println();
    }
}


int max_line_sensor(){
    /*int max_val = 1023, max_s = -1, tmp;

    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
        tmp = analogRead(line_sensors[i]);

        if(tmp < max_val){
          max_val = tmp;
          max_s = i;
        }

        if (max_val < line_values[i]){
            return max_s + 1;
        }
    }

    if (max_val > line_values[max_s]) max_s = -1;

    return max_s + 1;*/
    
    for(int i = 0; i < sizeof(line_sensors)/sizeof(*line_sensors); i++) {
        mutex[i] = 1;
        if (!digitalRead(line_sensors[i]) || !ws[i]) {
          ws[i] = 1;
          mutex[i] = 0;
          return i + 1;
          }
        mutex[i] = 0;
      }
     return 0; 
}

boolean check_light_sensors(){
  
  int line_sensor = max_line_sensor();
   if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
  // if (line_sensor != 0 && (v_action != ' ' || h_action != ' ')) {
        //PCICR = ~_BV(PCIE2);
        stopAllMotors();
    //if (ws[2] && ws[3] && ws[4] && ws[5]) {
  
   //if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
        switch (line_sensor){
            case 1:
            case 2://5,6
                if (v_action == 'U'){
                    back();
                }else if (v_action == 'B'){
                    up();
                }               
                delay(200); //250
                stopAllMotors();
                return true;
        }
   

        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                   // back_right();
                   motorA.stop();
                   motorB.go(speed-40);
                   motorC.go(-speed+40);
                }else if (v_action == 'B'){
                    // up_right();
                    motorA.go(speed-40);
                    motorB.stop();
                    motorC.go(-speed+40); 
                }else {
                    right();
                  }
                    break;

            case 'R':
                if (v_action == 'U'){
                    // back_left();
                    motorA.go(-speed+40);
                    motorB.stop();
                    motorC.go(speed-40);
                }else if (v_action == 'B'){
                    // up_left();
                     motorA.stop();
                     motorB.go(-speed+40);
                     motorC.go(speed-40);
                }else {
                    left();
                  }
                break; 
        }
        delay(150); //400, 200
        stopAllMotors();       
        return true;
    } else {
        return false;
    }
}
