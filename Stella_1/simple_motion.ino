void simple_motion_start()
{
    simple_motion_running = 1;
}

void simple_motion_stop()
{
    simple_motion_running = 0;
    stopAllMotors();
}

void simple_motion()
{
    uint8_t sensor = max_sensor();
  
    if(sensor == 1){
        if (ball_in_dribbler()) {
            kick();
            stopAllMotors();
            delay(500);
        } 
    
        motorA.go(speed);
        motorB.go(speed);
        motorC.go(speed);
    } else {
        stopAllMotors();
        motorB.go(speed);   
    }
}



