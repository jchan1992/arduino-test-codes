//#include <BufferedSerial.h>
//#include <ByteBuffer.h>
//
//BufferedSerial serial = BufferedSerial(256,256);
//ByteBuffer Send_Buffer;

#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

byte tx[3] = {0,0,0};
void motor(byte channel,byte mode,byte percent);
void debug();

void setup(){
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  Serial1.begin(9600);
//  serial.init(1,9600);
//  serial.setPacketHandler(handlePacket);
//  send_buffer.init(3);
}

void loop(){
 if(Serial1.available()>3){
   for(int i = 0;i<3;i++){
     tx[i] = Serial1.read();
      motor(tx[0],tx[1],tx[2]);
   }
    }else{
       Serial1.println("waiting for data");
       delay(500);
    }
}
  
void motor(byte channel,byte mode,byte percent){
  byte enable;
  byte gate1;
  byte gate2;
  byte per;
  
  if(channel == 1){
    enable = ENA;
    gate1 = IN1;
    gate2 = IN2;
  }else if(channel ==2){
    enable = ENB;
    gate1 = IN3;
    gate2 = IN4;
  }else{
    Serial1.println("invalid channel");
  }
  
  if(mode == 0){
    digitalWrite(enable,LOW);
  }else if(mode == 1){
    digitalWrite(gate1,HIGH);
    digitalWrite(gate2,LOW);
  }else if(mode == 2){
    digitalWrite(gate1,LOW);
    digitalWrite(gate2,HIGH);
  }else{
    Serial1.println("invalid mode");
  }
   
  per = map(percent,0,100,0,255);
  analogWrite(enable,per);  
}

void debug(){
  Serial1.print(digitalRead(ENA));
  Serial1.print("\t");
  Serial1.print(digitalRead(ENB));
  Serial1.print("\t");
  Serial1.print(digitalRead(IN1));
  Serial1.print("\t");
  Serial1.print(digitalRead(IN2));
  Serial1.print("\t");
  Serial1.print(digitalRead(IN3));
  Serial1.print("\t");
  Serial1.println(digitalRead(IN4));
}
