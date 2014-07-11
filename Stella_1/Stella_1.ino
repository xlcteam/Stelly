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
int speed=255;
int speed_min=60;
int speed_rotate=85;
int range = 7;
int kicker_time = 30;
int line_min_value = 150;

#define DRIBB_SENSOR_PORT 53

#define TSOP_PORT A4
#define TSOP_PORT2 A5

#define PORT_DRIBBLER 28
#define PORT_KICKER 2

#define US_LEFT_R_PIN 35
#define US_LEFT_W_PIN 34

#define BUTTON1 12 //motion
#define BUTTON2 44 //compass
#define BUTTON3 42 //kick
 
#define LED1 13 //motion
#define LED2 45 //compass
#define LED3 43 //kick

#define LEFT_NEAR 370
#define RIGHT_NEAR 390
#define REAR_NEAR 450

int line_sensors[] = {A8, A9, A10, A11, A12, A13, A14};
uint8_t ws[] = {0, 0, 0, 0, 0, 0, 0};
uint8_t mutex[] = {0, 0, 0, 0, 0, 0, 0};

Motor motorA = Motor(4, 5);
Motor motorB = Motor(7, 6);
Motor motorC = Motor(11, 10);
Motor dribbler = Motor(8, 9);

RelativeNXTCompass compass;
HTIRSeekerV2 seeker;

char v_action = ' ';
char h_action = ' ';

int motion_running = 0;
int compass_loop;
int simple_motion_running = 0;
int smotion_running = 0;
int rotate_running;

int dribbler_state = 0;
int kicking_running = 0;
int ultrasonic_running = 0;

int rotated = 0;
char todo;

int board_connected = 0;

unsigned long timer = 0;

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
    for (int i = 0; i < 7; i++){
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
  PCMSK2 = _BV(PCINT16) | _BV(PCINT17) | _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21) | _BV(PCINT22);
  pinMode(2, OUTPUT);
  analogWrite(2, 70);
  
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
    
  vic_fn_add("kd", &kicking_start);
  vic_fn_add("KD", &kicking_stop);
  
  vic_fn_add("uls", &ultrasonic_left_start);
  vic_fn_add("ULS", &ultrasonic_left_stop);
  
  /* variables which can be set via Serial */
  vic_var_set_bind("speed", "255", &speed);
  vic_var_set_bind("mspeed", "60", &speed_min);
  vic_var_set_bind("rspeed", "130", &speed_rotate);
  vic_var_set_bind("range", "16", &range);
  vic_var_set_bind("kick", "23", &kicker_time);   
}

void loop()
{  
  /*
  // maybe bluetooth in the future
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

  if (digitalRead(BUTTON1) == 1) {
    vic_println(motion_running);
    if (motion_running == 0 ){
      motion_start();
    } else {
      motion_stop();
    }
    
    while(digitalRead(BUTTON1) == 1); 
  }
  
  if (motion_running){
      motion();
  }else{    
    if (digitalRead(BUTTON2) == 1) {
      vic_println("compass loading");
      digitalWrite(LED2, HIGH);
      compass_load();
      while(digitalRead(BUTTON2) == 1); 
      digitalWrite(LED2, LOW);
    }  
    
    if (digitalRead(BUTTON3) == 1) {
      vic_println(kicking_running);
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
  
    if (ultrasonic_running){
      us_loop();
    }
    
    if (compass_loop){
      serial_compass();
    }
    
  }
}

