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
int speed_rotate=90; // 130
int range = 16;
int kicker_time = 30;

#define DRIBB_SENSOR_PORT 53

#define PORT_DRIBBLER 28
#define PORT_KICKER 2

#define US_LEFT_R_PIN 35
#define US_LEFT_W_PIN 34

#define BUTTON1 12 //motion
#define BUTTON2 51 //compass
#define BUTTON3 32 //kick
 
#define LED1 13 //motion
#define LED2 50 //compass
#define LED3 33 //kick

// touch1 - down, touch2 - up
#define TOUCH_IN 43
#define TOUCH_OUT 41
#define TOUCH2_IN 47
#define TOUCH2_OUT 45

#define LEFT_NEAR 370
#define RIGHT_NEAR 390
#define REAR_NEAR 320


#define PHOTOGATE A5

// sensors without 5th sensor
int sensors[] =  {0,  1,  2,  3,  5,  6,  7};
//int clearing[] = {-200,  -200,  -200,  -200,  -200,  -200,  -200};
int clearing[] = {5,  0,  0,  0,  0,  0,  0};

int line_sensors[] = {A2, A3, A4};

Motor motorA = Motor(4, 5);
Motor motorB = Motor(7, 6);
Motor motorC = Motor(11, 10);
Motor dribbler = Motor(8, 9);

RelativeNXTCompass compass;
HTIRSeekerV2 seeker;

char h_action = ' ';
char v_action = ' ';

int motion_running = 0;
int esensors_loop = 0;
int compass_loop;
int simple_motion_running = 0;
int smotion_running = 0;
int rotate_running;

int dribbler_state = 0;
int kicking_running = 0;
int ultrasonic_running = 0;

int rotated = 0;
char todo;

uint8_t dir, s1, s2, s3, s4, s5;

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

void setup()
{
   // buttons
  
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  
  pinMode(US_LEFT_R_PIN, INPUT);
  pinMode(US_LEFT_W_PIN, OUTPUT);
  
  pinMode(TOUCH_IN, INPUT);
  pinMode(TOUCH_OUT, OUTPUT);
  pinMode(TOUCH2_IN, INPUT);
  pinMode(TOUCH2_OUT, OUTPUT);
  
  
  /*pinMode(32, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(36, OUTPUT);*/
  
  digitalWrite(PORT_KICKER, 0);
  
  
  Serial.begin(115200);
  //Serial3.begin(115200);
  vic_init(115200);

  stopAllMotors();
  
  Wire.begin();

  //dribbler
  pinMode(PORT_DRIBBLER, OUTPUT);
  pinMode(PORT_KICKER, OUTPUT);
  
  compass.set_north();   

  stopAllMotors();
 
  
  
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
  
  vic_fn_add("mA", &mA);
  vic_fn_add("mB", &mB);
  vic_fn_add("mC", &mC);
  
  vic_fn_add("t", &test);
  
  vic_fn_add("lS", &line_sensors_all); 
  
  vic_fn_add("smu", &special_movement_up);
  vic_fn_add("smb", &special_movement_back);
  vic_fn_add("sml", &special_movement_left);
  vic_fn_add("smr", &special_movement_right);
  
  vic_fn_add("ibn", &is_ball_near);  
  
  vic_fn_add("kd", &kicking_start);
  vic_fn_add("KD", &kicking_stop);
  
  vic_fn_add("ul", &ultrasonic_left_start);
  vic_fn_add("UL", &ultrasonic_left_stop);
  
  vic_var_set_bind("speed", "255", &speed);
  vic_var_set_bind("mspeed", "60", &speed_min);
  vic_var_set_bind("rspeed", "200", &speed_rotate);
  vic_var_set_bind("range", "16", &range);
  vic_var_set_bind("kick", "23", &kicker_time);   
  

}


void loop()
{  
  //Serial.println("l");
 
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
  
  
  if (digitalRead(BUTTON2) == 1) {
    vic_println("compass loading");
    digitalWrite(LED2, HIGH);
    compass_load();
    while(digitalRead(BUTTON2) == 1); 
    digitalWrite(LED2, LOW);
  }  
  
  if (digitalRead(BUTTON3) == 1) {
    digitalWrite(LED3, HIGH);
    kick();
    
    while(digitalRead(BUTTON3) == 1); 
    digitalWrite(LED3, LOW);
  }
  
  
  if (motion_running) {
    digitalWrite(LED1, HIGH);

    motion();
  } else {
    analogWrite(LED1, LOW);
  }
  
  /*if (kicking_running) {
    digitalWrite(LED3, HIGH);
    kicking();
  } else {
    analogWrite(LED3, LOW);
  }*/
  
  if(simple_motion_running){
    analogWrite(LED1, HIGH);
    simple_motion();
  } else {
    analogWrite(LED1, LOW);
  }

  if (ultrasonic_running){
    us_loop();
  }

  if (esensors_loop) {
    esensors_all();
  }
  
  if (compass_loop){
    serial_compass();
  }
}

