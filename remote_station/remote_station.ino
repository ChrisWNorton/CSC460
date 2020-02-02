#include <SoftwareSerial.h>
#include <Servo.h>

// bt - at
// tx - rx1
// rx - tx1
// vcc - 5v
// gnd - gnd

// Brown - ground
// Red   - 5v
// orange- pin9 pmw

int ROTATE_PIN = 8;
int PITCH_PIN = 9;
//
Servo rotate_servo;
Servo pitch_servo;
//
//int pos = 90;
//
//void setup() {
//  rotate_servo.attach(ROTATE_PIN);
//  pith_servo.attach(PITCH_PIN);
//}
//
//void loop() {
//  for (;pos <= 135; pos += 1) {
//    rotate_servo.write(pos);
//    pitch_servo.write(pos);
//    delay(27);
//  }
//  for (;pos >= 45; pos -= 1) {
//    rotate_servo.write(pos);
//    pitch_servo.write(pos);
//    delay(28);
//  }
//}

int check_bounds(int pos, int mov_step) {
  if(pos >= 90 && mov_step>0){
    mov_step = 0;  
  }

  if(pos <= 50 && mov_step<0){
    mov_step = 0;  
  }
  return mov_step;
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

const int X_CENTER = 480;
const int Y_CENTER = 495;
const int NEUTRAL_BAND = 20;
// return step of rotation
int joystick_to_step(int input, int center) {
  if(input >= center + NEUTRAL_BAND){
    return 1;
  } else if(input <= center - NEUTRAL_BAND){
    return -1;
  }
  return 0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  rotate_servo.attach(ROTATE_PIN);
  pitch_servo.attach(PITCH_PIN);
}

int bytes_received = 0;
const int INPUT_SIZE = 11;
char bluetooth_input[INPUT_SIZE + 1];

// X,Y,SW
int control[3] = {1,1,1};

int rotate_step = 0;
int pitch_step = 0;

int rotate_pos = 60;
int pitch_pos = 0;

void loop() {
  if (Serial1.available()) {
    strcpy(bluetooth_input,"");
    Serial1.readBytesUntil('\n', bluetooth_input, INPUT_SIZE);
//    Serial.println(bluetooth_input);
    parse_input(control, bluetooth_input);
//    Serial.print(control[0]);
//    Serial.print(" ");
//    Serial.print(control[1]);
//    Serial.print(" ");
//    Serial.println(control[2]);
    rotate_step = joystick_to_step(control[0], X_CENTER);
    pitch_step = joystick_to_step(control[1], Y_CENTER);

//    Serial.print(rotate_step);
//    Serial.print(" ");
//    Serial.println(pitch_step);

    if(rotate_pos >= 90 && rotate_step>0){
      rotate_step = 0;  
    }

    rotate_step = check_bounds(rotate_pos, rotate_step);
    pitch_step = check_bounds(pitch_pos, pitch_step);
    rotate_pos = rotate_pos + rotate_step;
    pitch_pos = pitch_pos + pitch_step;


    rotate_servo.write(rotate_pos);
    pitch_servo.write(pitch_pos);
//    Serial.println(rotate_pos);
  }

  

}
