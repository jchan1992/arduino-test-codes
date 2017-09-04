#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2
#define BUFFER_SIZE 6
byte i = 0;
char rxBuffer[BUFFER_SIZE);

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
}

void loop(){
  if(Serial1.available()>0){
    Serial1.readBytes(rxBuffer,6);
//    Serial1.write([0]);
//    Serial1.write(tx[1]);
//    Serial1.write(tx[2]);
    motor(rxBuffer[0],rxBuffer[1],rxBuffer[2]);
    motor(rxBuffer[3],rxBuffer[4],rxBuffer[5]);
    
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
