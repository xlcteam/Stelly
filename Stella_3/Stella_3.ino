#include <Motor.h>
#include <Wire.h>
#include <NXTI2CDevice.h>
#include <RelativeNXTCompass.h>
#include <HTIRSeekerV2.h>
// if you want debug info from vic, uncomment line below
// #define DEBUG

#define VIC_ARDUINO_SERIAL Serial
#include <vic.h>

//Constants
uint8_t speed= 250;
uint8_t speed_min= 60;
uint8_t speed_rotate= 80; //110
uint8_t range = 10;
uint8_t kicker_time = 30;
// line_min_value = 150;

/* constant definitions*/
#define DRIBB_SENSOR_PORT 53

#define TSOP_PORT A7 // right 7
#define TSOP_PORT2 A6 // left 6

#define LIGHT_PWM 5

#define BUTTON1 22 //motion
#define BUTTON2 24 //compass
#define BUTTON3 26 //kick

#define LED1 23 //motion
#define LED2 25 //compass
#define LED3 27 //kick

int line_sensors[] = {A11, A8, A9, A12, A15, A14};
uint8_t ws[] = {0, 0, 0, 0, 0, 0};
uint8_t mutex[] = {0, 0, 0, 0, 0, 0};

Motor motorB = Motor(30, 8); // dir, pwm
Motor motorA = Motor(32, 7);
Motor motorC = Motor(34, 6);
Motor dribbler = Motor(36, 4);

RelativeNXTCompass compass;
HTIRSeekerV2 seeker;

char v_action = ' ';
char h_action = ' ';

uint8_t motion_running = 0;
uint8_t compass_loop;
uint8_t simple_motion_running = 0;
uint8_t dribbler_state = 0;

void test()
{
    int c = (int) compass.angle();
    if ( c > 0 && c < 180) {
        //Serial.println(speed_rotate-(speed_rotate*c/90));
        motorA.go(speed_rotate-(speed_rotate*c/180));
        motorB.go(speed_rotate-(speed_rotate*c/180));
        motorC.go(speed_rotate-(speed_rotate*c/180));
    } else {
        stopAllMotors();
        /*
        c -= 180;
      
        motorA.go(speed_rotate);
        motorB.go(speed_rotate-(speed_rotate*c/180));
        motorC.go(speed_rotate-(speed_rotate*c/180));
        motorD.go(speed_rotate);
        */
    }
}

ISR(PCINT2_vect)
{
    for (uint8_t i = 0; i < 6; i++){
        if (!mutex[i] && !ws[i] && digitalRead(line_sensors[i]))
            ws[i] = 1; 
    }
}

void setup()
{
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
  
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(BUTTON3, INPUT);
    
    PCICR = _BV(PCIE2);
    PCMSK2 = _BV(PCINT16) | _BV(PCINT17) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT22) | _BV(PCINT23);
    pinMode(LIGHT_PWM, OUTPUT);
    analogWrite(LIGHT_PWM, 200);  // 230 70, 57, 55, 35(este stale vidi), 30(niekedy vidi), 20, 60, 40, 45, 40
    
    Serial.begin(115200);
    //Serial3.begin(115200);
    vic_init(115200);
  
    stopAllMotors();
    
    Wire.begin();
    
    compass.set_north();   
  
    stopAllMotors();
   
    /* functions which are callable via Serial */
    vic_fn_add("dc", &compass_default);
    vic_fn_add("lc", &compass_load);
    
    vic_fn_add("d", &dribbler_on);
    vic_fn_add("D", &dribbler_off);
    
    vic_fn_add("ds", &serial_dribbler_sernsor);
    
    vic_fn_add("c", &serial_compass_loop_start);
    vic_fn_add("C", &serial_compass_loop_stop);
    vic_fn_add("cs", &serial_compass);
    
    vic_fn_add("k", &kick);
    
    vic_fn_add("m", &motion_start);
    vic_fn_add("M", &motion_stop);
  
    vic_fn_add("i", &simple_motion_start);
    vic_fn_add("I", &simple_motion_stop);
  
    vic_fn_add("s", &sensors_all);
    
    vic_fn_add("r", &right);
    vic_fn_add("l", &left);
    vic_fn_add("u", &up);
    vic_fn_add("b", &back);
    vic_fn_add("h", &halt);
  
    vic_fn_add("bl", &back_left);
    vic_fn_add("br", &back_right);
    
    vic_fn_add("ul", &up_left);
    vic_fn_add("ur", &up_right);
    
    vic_fn_add("mA", &mA);
    vic_fn_add("mB", &mB);
    vic_fn_add("mC", &mC);
    
    vic_fn_add("t", &test);
    
    vic_fn_add("lS", &line_sensors_all); 
    
    vic_fn_add("smu", &special_movement_up);
    vic_fn_add("smb", &special_movement_back);
    vic_fn_add("sml", &special_movement_left);
    vic_fn_add("smr", &special_movement_right);
    
    /* variables which can be set via Serial */
    vic_var_set_bind("speed", "255", &speed);
    vic_var_set_bind("mspeed", "60", &speed_min);
    vic_var_set_bind("rspeed", "130", &speed_rotate);
    vic_var_set_bind("range", "16", &range);
    vic_var_set_bind("kick", "23", &kicker_time);   
}

void loop()
{  
    /* maybe bluetooth in the future
    if(Serial3.available()){
      char a = Serial3.read();
      vic_process(a);
    }
    */

    if(Serial.available()){
        char a = Serial.read();
        vic_process(a);
    }
    vic_tasks_run(); 

    if (digitalRead(BUTTON1) == 1){ //&& (motion_running == 0 ) {
        vic_println(motion_running);
        motion_start();
       /* if (motion_running == 0 ){
            motion_start();
        } else {
            motion_stop();
        }

        while(digitalRead(BUTTON1) == 1); */
    }

    if (motion_running){
        motion();
    } else {    
        if (digitalRead(BUTTON2) == 1) {
            vic_println("compass loading");
            digitalWrite(LED2, HIGH);
            compass_load();
            while(digitalRead(BUTTON2) == 1); 
            digitalWrite(LED2, LOW);
        }  

        if (digitalRead(BUTTON3) == 1) {
            digitalWrite(LED3, HIGH);
            dribbler_on();
            while(digitalRead(BUTTON3) == 1); 
            digitalWrite(LED3, LOW);
            dribbler_off(); 
        }

        if(simple_motion_running){
            digitalWrite(LED1, HIGH);
            simple_motion();
        } else {
            analogWrite(LED1, LOW);
        }

        if (compass_loop){
            serial_compass();
        }
    }
}

