unsigned char read_adc(byte port)
{
  ADMUX = port | (0x60 & 0xff);
  delayMicroseconds(10);
  ADCSRA |= 0x40;
  while((ADCSRA & 0x10) == 0);
  ADCSRA |= 0x10;
  
  return ADCH;  
}

void esensors_loop_start()
{
  esensors_loop = 1;
}

void esensors_loop_stop()
{
  esensors_loop = 0;
}

int emax_sensor()
{
  int max_val = 253, max_s = -1, tmp;
  for(int i = 0; i < ( sizeof(sensors)/sizeof(*sensors)); i++){
    tmp = read_adc(sensors[i]); 
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

void esensors_all()
{

  int max_val = 253, max_s = -1, tmp;
  
  for(int i = 0; i < ( sizeof(sensors)/sizeof(*sensors)); i++){
    tmp = read_adc(sensors[i]);
    // TODO
    /*
    if (sensors[i] == 0)
      tmp += 100;
    */
    tmp -= clearing[i];
    
    //Serial3.print(tmp);
    //Serial3.print(" ");
    if(tmp < max_val){
      max_val = tmp;
      max_s = i;
    }
  }
  //Serial3.print(max_s + 1);
  //Serial3.println();
  
}


