 #include <LiquidCrystal.h>
 #include "scheduler.h"
 #include "arduino.h"

int led = 13;           // the PWM pin the LED is attached to
int pwm = 0;    // how bright the LED is
int fade_amount = 0;    // how many points to fade the LED by

int analog_vy_pin = A15;
int analog_vx_pin = A14;
int switch_pin = 53;
//One in Ground, one in digital PWM
int laser_pin = 13;
//One in ground, one in 5v
int photocell_pin = A13;

int vy = 0;
int vx = 0;
uint16_t idle_pin = 7;
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

const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(laser_pin, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  Scheduler_Init();
  Scheduler_StartTask(0,100,read_photocell_task);
  Scheduler_StartTask(0,100,read_stick_down_task);
  Scheduler_StartTask(0,100,read_stick_pos_task);
  Scheduler_StartTask(0,50,send_data_task);
  Scheduler_StartTask(0,100,print_to_lcd_task);
}

void sendData(int x, int y, bool switch_clicked){
  Serial1.print('>');
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

void print_to_lcd(int x, int y, int photosensor, bool switch_clicked){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("X:");
  lcd.print(x);
  lcd.print(" ");
  lcd.print("Click: ");
  if(switch_clicked){
    lcd.print("T");
  }else{
    lcd.print("F");
  }
  lcd.setCursor(0,1);
  lcd.print("Y:");
  lcd.print(y);
  lcd.print(" Photo:");
  lcd.print(photosensor);
  
}

void adjust_laser(bool switch_clicked){
   if(switch_clicked){
      analogWrite(laser_pin, 255);
   }else{
      analogWrite(laser_pin, 0);
   }
}

//Photocell
void read_photocell_task(){
  photocell_value = analogRead(photocell_pin);
  Serial.print("Photocell value: ");
  Serial.write(photocell_value);
  Serial.print('\n');
}

// idle task, from Neil's time triggered scheduling
void idle(uint32_t idle_period)
{
  // this function can perform some low-priority task while the scheduler has nothing to run.
  // It should return before the idle period (measured in ms) has expired.  For example, it
  // could sleep or respond to I/O.
 
  // example idle function that just pulses a pin.
  digitalWrite(idle_pin, HIGH);
  delay(idle_period);
  digitalWrite(idle_pin, LOW);
}

//Stick
void read_stick_down_task(){
  switch_value = digitalRead(switch_pin);
  
  if(switch_value == 0){
     num_switch_zeros++;
  }else if(switch_value == 1){
     num_switch_zeros = 0;
     switch_clicked = false;
     Serial.print("\nSwitch off\n");
  }
  if(num_switch_zeros >= 3){
    Serial.print("\nSwitch on\n");
    switch_clicked = true;
    num_switch_zeros = 0;
  }
  Serial.print("\nSwitch value: ");
  Serial.print(switch_value);
  Serial.print("\n");

}

//Vx, vy
void read_stick_pos_task() {
  vy = analogRead(analog_vy_pin);
  vx = analogRead(analog_vx_pin);
  
    Serial.write((char) vx);
//  Serial.print(" ");
//  Serial.print(vy);
//  Serial.print('\n');
}

//Send data
void send_data_task() {
  sendData(vx, vy, switch_clicked);
}

void print_to_lcd_task(){
  print_to_lcd(vx, vy, photocell_value, switch_clicked);
}

int exponential(int current_value, int previous_value){
  return (int)(alpha * (double)current_value) + ((1 - alpha) * (double)previous_value)  ;
}

void loop() {
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }

}
