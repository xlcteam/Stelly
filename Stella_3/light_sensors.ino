void line_sensors_all()
{  
    for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){    
        vic_print(digitalRead(line_sensors[i]));
        vic_print(" ");
    }
    vic_println();  
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
        PCICR = ~_BV(PCIE0);
        stopAllMotors();
    
        switch (line_sensor){
                case 5:
                case 6:
                back();
                delay(400); // 250
                PCICR = _BV(PCIE0);
                return true;
        }
    
        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                  // back right();  
                    motorA.stop();
                    motorB.go(-speed+56);
                    motorC.go(speed-60);
                }else if (v_action == 'B'){
                   // up_right();
                   motorA.go(-speed+50);
                   motorB.stop();
                   motorC.go(speed-50);
                }
                break;

            case 'R':
                if (v_action == 'U'){
                   //back_left();
                   motorA.go(speed-60);
                   motorB.stop();
                   motorC.go(-speed+60);
                }else if (v_action == 'B'){
                    //up_left();
                    motorA.stop();
                    motorB.go(speed-56);
                    motorC.go(-speed+60);
                }
                break; 
        }
        delay(300); // 400
        stopAllMotors();
        PCICR = _BV(PCIE0);
        return true;
    } else {
        return false;
    }
}
