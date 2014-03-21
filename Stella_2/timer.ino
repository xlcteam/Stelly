
void start_timer()
{
  if(timer == 0)
    timer = millis();
}


boolean check_timer(unsigned long t)
{
  if(timer + t < millis() &&  timer != 0){
    timer = 0;
    return true;
  } else {
    return false;
  }
}

