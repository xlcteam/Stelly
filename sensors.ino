int max_sensor()
{
  int max_val = 1003, max_s = -1, tmp;
  for(int i = 0; i < ( sizeof(sensors)/sizeof(*sensors)); i++){
    tmp = analogRead(sensors[i]); 
    // TODO
    /*
    if (sensors[i] == 0)
      tmp += 100;
    */
    
    tmp -= clearing[i];
    
    if(tmp < max_val){
      max_val = tmp;
      max_s = i;
    }
  }
  return max_s + 1;
}

void sensors_all()
{

  int max_val = 1023, max_s = -1, tmp;
  
  for(int i = 0; i < ( sizeof(sensors)/sizeof(*sensors)); i++){
    tmp = analogRead(sensors[i]);
    // TODO
    /*
    if (sensors[i] == 0)
      tmp += 100;
    */
    tmp -= clearing[i];
    
    vic_print(tmp);
    vic_print(" ");
    if(tmp < max_val){
      max_val = tmp;
      max_s = i;
    }
  }
  vic_print(max_s + 1);
  vic_println();
  
}


void serial_dribbler_sernsor()
{
  vic_func();
  vic_return("%d", ball_in_dribbler());
}



