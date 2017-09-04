//float send uno 

typedef union{
 float num;
 byte bite[4];
} byteFloat;

const int pwmPin = 3;
byteFloat cm3;
float data3 = 0;

void setup(){
  Serial.begin115200;
  cm3.num = 0;
}

void loop(){
  data3 = pulseIn(pwmPin,HIGH);
  cm3.num = data3/147*2.54;

  
  Serial.write(cm3.bite,4);
}
          
