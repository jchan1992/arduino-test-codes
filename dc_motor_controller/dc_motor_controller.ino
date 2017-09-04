#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

#define RX_BUFFER_SIZE 256
#define COMMAND_BUFFER_SIZE 7
#define BAUD_RATE 9600
#define START 0xFF

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

char rxBuffer[RX_BUFFER_SIZE];
char commandBuffer[COMMAND_BUFFER_SIZE];
uint8_t rxDataCount = 0;
uint8_t counter = 0;

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
  counter = 0;
  rxDataCount = 0;
   if(Serial1.available()>0){
     rxDataCount = Serial1.available();
     if(rxDataCount > 6){
       Serial1.print("data on rx:");
       Serial1.println(rxDataCount);
       Serial1.readBytes(rxBuffer,rxDataCount);
     }else{
       Serial1.println("not enough data to decode");
       return;
     }
     for(counter;counter < rxDataCount;counter++){
       if(rxBuffer[counter] == START){
         Serial1.print("found");
         Serial1.print(rxBuffer[counter]);
         Serial1.print(" at ");
         Serial1.println(counter);
         break;
       }else{
         Serial1.print(counter);
         Serial1.print("\t");
         Serial1.println(rxBuffer[counter]);
       }
     }
//     while(rxBuffer[counter] != 0xFF){
//       counter++;
//       Serial1.println("searching...");
//     }
     for(uint8_t i = 0; i<COMMAND_BUFFER_SIZE;i++){
      commandBuffer[i] =  rxBuffer[counter];
      Serial1.println(commandBuffer[i]);
      counter++;
     }
     if(commandBuffer[0] != 255){
       Serial1.println("I messed up!");
     }else{
       motorcontrol(commandBuffer[1],commandBuffer[2],commandBuffer[3]);
       motorcontrol(commandBuffer[4],commandBuffer[5],commandBuffer[5]);
     }
   }else{
     Serial1.println("Waiting for data...");
     delay(50);
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
  
  
          
          
