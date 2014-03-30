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
  
  if (max_val > 340) max_s = -1;
  
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
  
  if (max_val > 340) max_s = -1;
  
  return max_s + 1;

}

void check_light_sensors()
{
  int line_sensor = max_line_sensor();
  if (line_sensor != 0 && action != ' ') {
    switch (action) {
      case 'L':
        right();
        /*Serial.print(action);
        Serial.print(" ");    
        Serial.println("from white");*/
        delay(400);        
        return;
        break;
        
      case 'R':
        left();
        /*Serial.print(action);
        Serial.print(" ");    
        Serial.println("from white");*/
        delay(400);        
        return;
        break;    
        
    /*  case 'U':
        back();
        break;
      
      case 'B':
        up();
        break;    */  
    }

  }
}
