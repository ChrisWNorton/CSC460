int led = 13;           // the PWM pin the LED is attached to
int pwm = 0;    // how bright the LED is
int fade_amount = 0;    // how many points to fade the LED by

int analog_vy_pin = A0;
int analog_vx_pin = A1;
int switch_pin = 53;

int vy = 0;
int vx = 0;
int previous_x = -1;
int previous_y = -1;
int switch_value = 0;
int num_switch_zeros = 0;
bool switch_clicked = false;
double alpha = 0.2;
int diff;

int MID_VAL = 520;
int NOISE_BAND = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(led, OUTPUT);
  
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

int exponential(int current_value, int previous_value){
  return (int)(alpha * (double)current_value) + ((1 - alpha) * (double)previous_value)  ;
}

void loop() {
  // put your main code here, to run repeatedly:
  vy = analogRead(analog_vy_pin);
  vx = analogRead(analog_vx_pin);
  switch_value = digitalRead(switch_pin);

  Serial.print(previous_x);
  Serial.print(" ");
  Serial.print(vx);
  Serial.print(" ");
  
  if(previous_x != -1){
    vx = exponential(vx, previous_x);
    vy = exponential(vy, previous_y);
  }

  if(vx >= MID_VAL - NOISE_BAND && vx <= MID_VAL + NOISE_BAND){
    vx = MID_VAL;
  }

  if(vy >= MID_VAL - NOISE_BAND && vy <= MID_VAL - NOISE_BAND){
    vy = MID_VAL;
  }
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
  
  analogWrite(led, pwm);

  // scale fade amount base off of 0/520 ratio
  fade_amount = (double(abs(vx - MID_VAL)) / double(MID_VAL)) * double(30);

   // change the brightness for next time through the loop:
  pwm += fade_amount;

  // reverse the direction of the fading at the ends of the fade:
  if (pwm <= 0 || pwm >= 255) {
    fade_amount = -fade_amount;
  }
  
  sendData(vx, vy, switch_clicked);
  
  delay(30);
  
  Serial.print("\n");

}
