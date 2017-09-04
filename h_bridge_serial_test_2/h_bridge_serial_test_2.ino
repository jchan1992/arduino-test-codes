#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

#define BUFFER_LG 20

char rxBuffer[BUFFER_LG];
void motor(byte channel,byte mode,byte percent);
void debug();
int rxReceivedCount;

void setup(){
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  
  Serial1.begin(9600);
  
  rxReceivedCount = 0;
}

void loop(){
  
  rxReceivedCount = Serial1.available();
  
  if(rxReceivedCount > 6){
    Serial1.readBytes(rxBuffer,rxReceivedCount);
    
    Serial1.write(11);
    Serial1.write(rxReceivedCount);
  
    int i = 0;
    boolean headerDetected = false;
    while (!headerDetected && (i < rxReceivedCount)){
      if (rxBuffer[i] == 254) headerDetected = true;
      Serial1.write(22);
      Serial1.write(rxBuffer[i]);
      i++;
    }
    
    if (headerDetected && ((rxReceivedCount - i) > 5)) {
      
      Serial1.write(33);
      Serial1.write(rxBuffer[i]);
      
      motor(rxBuffer[i],rxBuffer[i+1],rxBuffer[i+2]);
      motor(rxBuffer[i+3],rxBuffer[i+4],rxBuffer[i+5]);
    }
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
