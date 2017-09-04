//float send mega

typedef union{
 float num;
 byte bite[4];
} byteFloat;

const int pwmPin = 2;
byteFloat sen;
float data = 0;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  sen.num = 0;
}

void loop(){
  data = pulseIn(pwmPin,HIGH);
  sen.num = data/147*2.54;
  
  Serial.println(sen.num);
  
  for(int i = 0;i<4;i++){
    Serial.write(sen.bite[i]);
  }
  
}
