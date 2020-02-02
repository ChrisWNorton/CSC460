int led = 13;           // the PWM pin the LED is attached to
int pwm = 0;    // how bright the LED is
int fade_amount = 0;    // how many points to fade the LED by

int analog_vy_pin = A1;
int analog_vx_pin = A0;
int switch_pin = 53;
//One in Ground, one in digital PWM
int laser_pin = 13;
//One in ground, one in 5v
int photocell_pin = A2;

int vy = 0;
int vx = 0;
int previous_x = -1;
int previous_y = -1;
int switch_value = 0;
int num_switch_zeros = 0;
bool switch_clicked = false;
double alpha = 0.1;
int diff;
int photocell_value;

int MID_VAL = 520;
int NOISE_BAND = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(laser_pin, OUTPUT);
  
}

void sendData(int x, int y, bool switch_clicked){
  Serial1.print(x);
  Serial1.write(',');
  Serial1.print(y);
  Serial1.write(',');
  if(switch_clicked){
    Serial1.print(1);
  }else{
    Serial1.print(0);
  }  
  Serial1.write('\n');
}

void adjust_laser(bool switch_clicked){
   if(switch_clicked){
      analogWrite(laser_pin, 255);
   }else{
      analogWrite(laser_pin, 0);
   }
}

int exponential(int current_value, int previous_value){
  return (int)(alpha * (double)current_value) + ((1 - alpha) * (double)previous_value)  ;
}

void loop() {
  // put your main code here, to run repeatedly:
  vy = analogRead(analog_vy_pin);
  vx = analogRead(analog_vx_pin);
  switch_value = digitalRead(switch_pin);
  photocell_value = analogRead(photocell_pin);

  Serial.print(previous_x);
  Serial.print(" ");
  Serial.print(vx);
  Serial.print(" ");
  Serial.print("Photocell value: ");
  Serial.print(photocell_value);
  
//  if(previous_x != -1){
//    vx = exponential(vx, previous_x);
//    vy = exponential(vy, previous_y);
//  }
//
//  if(vx >= MID_VAL - NOISE_BAND && vx <= MID_VAL + NOISE_BAND){
//    vx = MID_VAL;
//  }
//
//  if(vy >= MID_VAL - NOISE_BAND && vy <= MID_VAL - NOISE_BAND){
//    vy = MID_VAL;
//  }
  Serial.print(previous_x);
  Serial.print(" ");
  Serial.print(previous_y);
  Serial.print(" ");
  previous_x = vx;
  previous_y = vy;
  
  
  Serial.print(vx);
  Serial.print(" ");
  Serial.print(vy);
  Serial.print("Switch value: ");
  Serial.print(switch_value);
  if(switch_value == 0){
    num_switch_zeros++;
  }else if(switch_value == 1){
    num_switch_zeros = 0;
     switch_clicked = false;
     Serial.print("Switch off");
  }

  if(num_switch_zeros == 6){
    Serial.print("Switch on");
    switch_clicked = true;
    num_switch_zeros = 0;
  }
  
  sendData(vx, vy, switch_clicked);
  adjust_laser(switch_clicked);
  
  
  delay(30);
  
  Serial.print("\n");

}
