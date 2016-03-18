void line_sensors_all()
{
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
    Serial.println();
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

        if (max_val < line_values[i]){
            return max_s + 1;
        }
    }

    if (max_val > line_values[max_s]) max_s = -1;

    return max_s + 1;
}

boolean check_light_sensors()
{
    int line_sensor = max_line_sensor();
    if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
        switch (line_sensor){
            case 1:
            case 2:
                if (v_action == 'U')
                    back();
                else if (v_action == 'B')
                    up();
                delay(250);
                return true;
        }

        switch (h_action) {
            case 'L':
                if (v_action == 'U'){
                   // back_right();
                   motorA.stop();
                   motorB.go(speed-50);
                   motorC.go(-speed+50);
                }
                else if (v_action == 'B'){
                    // up_right();
                    motorA.go(speed-50);
                    motorB.stop();
                    motorC.go(-speed+50); 
                }
                break;

            case 'R':
                if (v_action == 'U'){
                    // back_left();
                    motorA.go(-speed+50);
                    motorB.stop();
                    motorC.go(speed-50);
                }
                else if (v_action == 'B'){
                    // up_left();
                     motorA.stop();
                     motorB.go(-speed+50);
                     motorC.go(speed-50);
                }
                break; 
        }
        delay(400);
        stopAllMotors();       
        return true;
    } else {
        return false;
    }
}
