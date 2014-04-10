void line_sensors_all()
{

  int max_val = 1023, max_s = -1, tmp;
  
  for(int i = 0; i < ( sizeof(line_sensors)/sizeof(*line_sensors)); i++){
    tmp = analogRead(line_sensors[i]);
    // TODO
    /*
    if (sensors[i] == 0)
      tmp += 100;
    */
    
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

    // manduchs ducharina
    /*if (i == 1){
      digitalWrite(LED3, !digitalRead(LED3));
    }*/

    if(tmp < max_val){
      max_val = tmp;
      max_s = i;
    }
  }
  
  if (max_val > line_min_value) max_s = -1;
  
  return max_s + 1;

}

boolean check_light_sensors()
{
  int line_sensor = max_line_sensor();
  if (line_sensor != 0 && v_action != ' ' && h_action != ' ') {
    switch (h_action) {
      case 'L':
        if (v_action == 'U')
          back_right();
        else if (v_action == 'B')
          up_right();
        delay(400);
        stopAllMotors();        
        return true;
        break;
        
      case 'R':
        if (v_action == 'U')
          back_left();
        else if (v_action == 'B')
          up_left();
        left();
        delay(400);
        stopAllMotors();       
        return true;
        break; 
    }
  } else {
    return false;
  }
}
