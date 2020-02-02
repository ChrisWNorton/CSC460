#include <SoftwareSerial.h>

#include <Servo.h>

// Brown - ground
// Red   - 5v
// orange- pin9 pmw

int ROTATE_PIN = 8;
int PITCH_PIN = 9;

Servo rotate_servo; 
Servo pitch_servo;

int pos = 90;    

void setup() {
  rotate_servo.attach(ROTATE_PIN);
  pith_servo.attach(PITCH_PIN);
}

void loop() {
  for (;pos <= 135; pos += 1) {
    rotate_servo.write(pos);   
    pitch_servo.write(pos);           
    delay(27);
  }
  for (;pos >= 45; pos -= 1) {
    rotate_servo.write(pos);  
    pitch_servo.write(pos);            
    delay(28);                     
  }
}

////bt - at
////tx - rx1
////rx - tx1
////vcc - 5v
////gnd - gnd
//void setup() {
//  // put your setup code here, to run once:
//  Serial.begin(9600); 
//  Serial1.begin(9600);
//   pinMode(41, OUTPUT);
//}
//
//int bytes_received = 0;
//
//void loop() {
//  // put your main code here, to run repeatedly:
//  if(Serial.available()){ 
//    digitalWrite(41, HIGH);
//    Serial.read();
//    Serial1.print("12345678901234567890123456789012"); 
//  }
//  if(Serial1.available()){ 
//    Serial.print((char)Serial1.read());
//    bytes_received += 1;
//    if(bytes_received == 32){
//      digitalWrite(41, LOW);
//      Serial.print("All Received\n");  
//      bytes_received = 0;  
//    }
//    
//  }
//
//}
