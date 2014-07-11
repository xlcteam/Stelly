void kick()
{  
    digitalWrite(TOUCH_OUT, HIGH);
    while(digitalRead(TOUCH_IN)== 0){
        dribbler.go(-255);
    }
    dribbler.stop();
    digitalWrite(TOUCH_OUT, LOW);
    
    digitalWrite(TOUCH2_OUT, HIGH);
    while(digitalRead(TOUCH2_IN) == 1){
        dribbler.go(100);
    }
    dribbler.stop();
    digitalWrite(TOUCH2_OUT, LOW);
}


