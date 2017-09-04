#include <Servo.h>

Servo myServo;
const int signalPin = 9;
const int currentPin = A0;
float volt = 0;
const float ohm = 1;
float amp = 0;
float volta = 0;

void setup(){
  Serial.begin(9600);
  analogReference(DEFAULT);
  //pinMode(signalPin,OUTPUT);
  //pinMode(currentPin,INPUT);
  myServo.attach(signalPin);
  myServo.write(0);
}

void loop(){
  for(int i = 0;i<180;i++){
    myServo.write(i);
    volt = analogRead(currentPin);
    Serial.print(volt);
    Serial.print("\t");
    volta = map(volt,0,1023,0,5);
    amp = volta/ohm;
    Serial.print(volta);
    Serial.print("\t");
    Serial.println(amp);
    delay(0);
  }
//    for(int i = 180;i>0;i--){
//    myServo.write(i);
//    volt = analogRead(currentPin);
//    
//    amp = volt/ohm;
//    Serial.println(amp);
//    delay(50); 
//  }  
}
