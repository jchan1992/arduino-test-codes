#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

#define RX_BUFFER_SIZE 256
#define COMMAND_BUFFER_SIZE 7
#define BAUD_RATE 9600
#define START_BYTE 0xFF

#define LEFT 1
#define RIGHT 2

#define NOMO 0
#define CW 1
#define CCW 2

typedef struct motor{
    uint8_t enable;
    uint8_t gate1;
    uint8_t gate2;
    uint8_t throttle;
}motor;

unsigned char rxBuffer[RX_BUFFER_SIZE];
unsigned char commandBuffer[COMMAND_BUFFER_SIZE];
uint8_t rxCount = 0;
uint8_t count = 0;
uint8_t commandCount = 0;

void motorinit(void);
void motorstop(void);
void motorcontrol(uint8_t, uint8_t,uint8_t);
void motorstep(uint8_t,uint8_t);

void setup(){
  Serial1.begin(BAUD_RATE);
  Serial1.println("starting...");
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  motorinit();
  Serial1.println("beginning program");
}

void loop(){
//  if there is data
//    copy data into buffer
//    if data is not enough
//      keep reading
//    if data is enough
//      if start byte is found 
//        if there is enough data after start byte
//          make command
//        if there isnt enough data after start byte
//          keep reading
//      if start byte is not found
//        keep reading
//  if there isnt data 
//    wait for data
    
  rxCount = 0;
  commandCount = 0;
  count = 0;
  
  if(Serial1.available()>0){
    Serial1.print(Serial1.available());
    Serial.print("\t");
    rxCount = Serial1.available();
    Serial1.println(rxCount);
    for(int i = 0;i<rxCount;i++){
      rxBuffer[i] = Serial1.read();
    }
    //Serial1.readBytes(rxBuffer,rxCount);
    for(int i = 0;i<rxCount-1;i++){
      Serial1.print(rxBuffer[i],DEC);
      Serial1.print("\t");
    }
    Serial1.println(rxBuffer[rxCount-1],DEC);
    if(rxCount>6){
      Serial1.println("got enough data");
      for(count = 0; count < rxCount;count++){
        if(rxBuffer[count] != START_BYTE){
          Serial1.print(rxBuffer[count]);
          Serial1.print(" at ");
          Serial1.println(count);
        }else{
          break;
        }
      }
      Serial1.print("current count");
      Serial1.print(count);
      Serial1.print ("\t");
      Serial1.println(rxBuffer[count]);
      if(rxCount - count > 6){
        for(int i = 0; i <COMMAND_BUFFER_SIZE ;i++){
          commandBuffer[commandCount] = rxBuffer[count];
          Serial1.print(commandBuffer[commandCount]);
          Serial1.print(" at ");
          Serial1.println(commandCount);
          commandCount++;
          count++;
        } 
        motorcontrol(commandBuffer[1],commandBuffer[2],commandBuffer[3]);
        motorcontrol(commandBuffer[4],commandBuffer[5],commandBuffer[6]);
      }else{
         Serial1.println("not enough data to make command 2");
      }     
    }else{
      Serial1.println("not enough data to make command 1");
    }
  }else{
    Serial1.println("no incoming data");
  }
}
   
void motorinit(){
  motorstop();
}

void motorstop(){
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
}

void motorcontrol(uint8_t chan,uint8_t dir,uint8_t throt){
  
  motor myMotor;
  
   switch(chan){
      case LEFT: myMotor.enable = ENA;
                 myMotor.gate1 = IN1;
                 myMotor.gate2 = IN2;
      break;      
      case RIGHT: myMotor.enable = ENB;
                  myMotor.gate1 = IN3;
                  myMotor.gate2 = IN4;
      break;
      default: Serial1.println("invalid channel");
      break;
   }
   
   switch(dir){
     case NOMO: digitalWrite(myMotor.enable,LOW);
     break;
     case CW: digitalWrite(myMotor.gate1,HIGH);
              digitalWrite(myMotor.gate2,LOW);
     break;
     case CCW: digitalWrite(myMotor.gate1,LOW);
               digitalWrite(myMotor.gate2,HIGH);
     break;
     default: Serial1.println("invalid direction");
   }
   
   if(throt>=0 && throt<=100){
     myMotor.throttle = map(throt,0,100,0,255);
     if(myMotor.throttle<20){
       Serial1.write("not enough throttle");
     }else{
       analogWrite(myMotor.enable,myMotor.throttle);
     }
   }else{
     Serial1.write("invalid throttle input");
   }
}

void motorstep(uint8_t chan,uint8_t dir){
  motorcontrol(chan,dir,25);
  delayMicroseconds(200);
  motorcontrol(chan,dir,0);
}
  
  
          
          
