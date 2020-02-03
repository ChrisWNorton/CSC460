#include <SoftwareSerial.h>
#include <Servo.h>
#include "scheduler.h"
#include "arduino.h"

// bt - at
// tx - rx1
// rx - tx1
// vcc - 5v
// gnd - gnd

// Brown - ground
// Red   - 5v
// orange- pin9 pmw

// ground
// pwm 13

//One in Ground, one in digital PWM
const int LASER_PIN = 13;
const int ROTATE_PIN = 8;
const int PITCH_PIN = 9;
const int BT_INPUT_SIZE = 11;


// ------------------------------------------
// Bluetooth

char bluetooth_input[BT_INPUT_SIZE + 1];
// X,Y,SW
int control[3] = {1,1,1};

void read_bluetooth() {
  if (Serial1.available()) {
    // NEED TO READ LATESET VALUES
    strcpy(bluetooth_input,"");
    Serial1.readBytesUntil('\n', bluetooth_input, INPUT_SIZE);
    Serial.print(bluetooth_input);
    Serial.print("-");
    parse_input(control, bluetooth_input);
  }
}

void parse_input(int *output, char *bluetooth_input){
  char* token;
  char seps[] = ",";
  int var;
  int i = 0;

  token = strtok(bluetooth_input, seps);
  while (token != NULL)
  {
    sscanf (token, "%d", &var);
    output[i++] = var;

    token = strtok (NULL, seps);
  }
}

// ------------------------------------------
// Servo

Servo rotate_servo;
Servo pitch_servo;
const int X_CENTER = 480;
const int Y_CENTER = 495;
const int NEUTRAL_BAND = 20;

int rotate_step = 0;
int pitch_step = 0;

int rotate_pos = 60;
int pitch_pos = 60;

void servo_task(){
  rotate_step = joystick_to_step(control[0], X_CENTER);
  pitch_step = joystick_to_step(control[1], Y_CENTER);

  if(check_bounds(rotate_pos, rotate_step)){
    rotate_pos += rotate_step;
  }
  if(check_bounds(pitch_pos, pitch_step)){
    pitch_pos += pitch_step;
  }

  rotate_servo.write(rotate_pos);
  pitch_servo.write(pitch_pos);
}
// return step of rotation
int joystick_to_step(int input, int center) {
  if(input >= center + NEUTRAL_BAND){
    return 1;
  } else if(input <= center - NEUTRAL_BAND){
    return -1;
  }
  return 0;
}
int is_pos_inbound(int pos, int mov_step) {
  if(pos >= 90 && mov_step>0){
    return 0;  
  }
  if(pos <= 50 && mov_step<0){
    return 0;  
  }
  return 1;
}

// ------------------------------------------
// Laser

void laser_task(){
  adjust_laser(control[2]);
}
void adjust_laser(bool switch_clicked){
   if(switch_clicked){
      analogWrite(LASER_PIN, 255);
   }else{
      analogWrite(LASER_PIN, 0);
   }
}

// ------------------------------------------
// Main

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
  rotate_servo.attach(ROTATE_PIN);
  pitch_servo.attach(PITCH_PIN);

  Scheduler_Init();
  Scheduler_StartTask(0, 500, bluetooth_input);
  Scheduler_StartTask(0, 500, servo_task);
  Scheduler_StartTask(0, 500, laser_task);
  
}

void loop() {
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    delay(idle_period);
  }
}
