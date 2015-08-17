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
  
    if (line_sensor != 0 && (v_action != ' ' || h_action != ' ')) {
        //PCICR = ~_BV(PCIE2);
        stopAllMotors();
    
        if (ws[2] && ws[3] && ws[4] && ws[5]) {
            switch (line_sensor){
                    case 1:
                    case 2:
                        back();
                        delay(100); //250, 90(with compensation)
                        //PCICR = _BV(PCIE2);
                        return true;
                     default:
                         break;
            }
        }
    
        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                    //back_right();  
                    //motorA.go(compensation() );
                    motorA.stop();
                    motorB.go(speed - 50);
                    motorC.go(-speed + 50);
                }else if (v_action == 'B'){
                    //up_right();
                    motorA.go(speed - 50);
                    //motorB.go(compensation() );
                    motorB.stop();
                    motorC.go(-speed + 50); 
                }
                break;

            case 'R':
                if (v_action == 'U'){
                   //back_left();
                    motorA.go(-speed + 50);
                    //motorB.go(compensation() );
                    motorB.stop();
                    motorC.go(speed - 50);
                }else if (v_action == 'B'){
                    //up_left();
                     //motorA.go(compensation() );
                     motorA.stop();
                     motorB.go(-speed + 50);
                     motorC.go(speed - 50);
                }
                break;
        }
        delay(120); // 110, 450, 100(with compensation)
        stopAllMotors();
        //PCICR = _BV(PCIE2);
        return true;
    } else {
        return false;
    }
}
