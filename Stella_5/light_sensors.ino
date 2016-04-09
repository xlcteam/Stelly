void line_sensors_all(){
  while (1 == 1){  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){    
        //vic_print(digitalRead(line_sensors[i]));
        //vic_print(" ");
        Serial.print(digitalRead(line_sensors[i]));
        Serial.print(" ");
    }
    //vic_println();  
    Serial.println();
  }
}

int max_line_sensor()
{  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
        mutex[i] = 1; 
        if (!digitalRead(line_sensors[i]) || !ws[i]){
            ws[i] = 1; 
            mutex[i] = 0; 
            return i + 1;
        }
        mutex[i] = 0;
    }
    return 0;
}

boolean check_light_sensors()
{
    int line_sensor = max_line_sensor();
  
    if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
        //PCICR = ~_BV(PCIE2);
        stopAllMotors();
    
        switch (line_sensor){
                case 1:
                case 2:
                  if (v_action == 'B') {
                    up();
                  } else { back();
                }
                //back();
                delay(100); //250
                //PCICR = _BV(PCIE2);
                return true;
        }
    
        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                  // back_right();  
                    motorA.stop();
                    motorB.go(-speed + 50); // +70
                    motorC.go(speed - 50); // -70
                }else if (v_action == 'B'){
                   // up_right();
                   motorA.go(-speed + 50); // +70
                   motorB.stop();
                   motorC.go(speed - 50);// -70
                }
                else {
                   right();
                }
                break;

            case 'R':
                if (v_action == 'U'){
                   //back_left();
                   motorA.go(speed - 50); //-70
                   motorB.stop();
                   motorC.go(-speed + 50); //+70
                }else if (v_action == 'B'){
                    //up_left();
                    motorA.stop();
                    motorB.go(speed - 50); // -70
                    motorC.go(-speed + 50); //+70
                }else {
                    left(); 
                }
                break; 
        }
        delay(90); //450, 200, 100
        stopAllMotors();
        //PCICR = _BV(PCIE2);
        return true;
    } else {
        return false;
    }
}
