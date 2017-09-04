//define pins for h bridge
#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2
//define constants
#define NULL '\0'
#define VS_GRADIENT 11
#define RX_SIZE 256
#define BAUD_RATE 9600
#define STEP_PWM 120
#define STOP_PWM 0
#define LEFT 1
#define RIGHT 2
#define CW 1
#define CCW 2
#define COMMAND_BUFFER_0 7
#define COMMAND_BUFFER_1 8
#define COMMAND_BUFFER_2 9
typedef struct motor_t{
    uint8_t enable;
    uint8_t gate1;
    uint8_t gate2;
    uint8_t throttle;
    uint8_t time;
}motor_t;

//typedef enum  {left;
//              right
//}channel_t;

//typedef enum (cw;
//              ccw;
//}mode_t;

//typedef enum (commandBufferSize0 = 7;
//              commandBufferSize1 = 8;
//              commandBufferSize2 = 9;
//}commandBufferSize_t;

uint8_t rxSize = 0;
uint8_t rxCount = 0;
uint8_t commandCount = 0;
uint8_t commandSize = 0;

void motorStop(uint8_t);
void motorStep(uint8_t,uint8_t);
void motorSpin(uint8_t,uint8_t,uint8_t);

void setup(){
  //initialise motor
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  //initialise bluetooth
  Serial1.begin(BAUD_RATE);
  Serial1.println("program start");
}

void loop(){
  //check for data available
  if(Serial1.available()>0){
    Serial1.println("available data to read");
    //copy data into rx buffer
    rxSize = Serial1.available();
    Serial1.println(rxSize);
    unsigned char rxBuffer[RX_SIZE];
    for(int i = 0;i<rxSize;i++){
      rxBuffer[i] = Serial1.read();
    }
  
    ////look at bytes in rx buffer 
    Serial1.println("rx buffer array");
    for(int i = 0; i<rxSize; i++){
      Serial1.print(rxBuffer[i],DEC);
      Serial1.print(" at ");
      Serial1.println(i);
    }
    
    //search for start bytes
    boolean isStartFound = false;
    rxCount = 0;
    Serial1.println("searching for start");
    
    while(isStartFound = false && rxCount < rxSize){ 
     Serial.println("entering while"); 
      if(rxBuffer[rxCount] != 0xFF && rxBuffer[rxCount+1] !=0xFE && rxBuffer[rxCount+2] != 0xFD){
        Serial1.print(rxBuffer[rxCount]);
        Serial1.print("\t");
        Serial1.print(rxBuffer[rxCount+1]);
        Serial1.print("\t");
        Serial1.println(rxBuffer[rxCount+2]);
        rxCount++;
      }else{
        isStartFound = true;
        Serial1.println("found start bytes");
      }
    }
    
    if(isStartFound = true){
      //check command byte
      Serial1.println(rxBuffer[rxCount+3]);
      boolean isCommandValid = false;
      if(rxBuffer[rxCount+3]>=0 && rxBuffer[rxCount+3]<=2){
        isCommandValid = true;
      }
      
      if(isCommandValid = true){
        //determine command size
        uint8_t commandBufferSize;
        switch(rxBuffer[rxCount+3]){
        case 0:
        commandBufferSize = COMMAND_BUFFER_0;
        break;
        case 1:
        commandBufferSize = COMMAND_BUFFER_1;
        break;
        case 2:
        commandBufferSize = COMMAND_BUFFER_2;
        break;
        default:
        Serial1.println("invalid command");
        break;
        }      
        
        //make command buffer
        unsigned char commandBuffer[commandBufferSize];
        for(int i = 0; i<commandBufferSize;i++){
          commandBuffer[i] = rxBuffer[rxCount];
         rxCount++;
        }
    
        ////look at command buffer
        Serial1.println("command buffer array");
        for(int i = 0;i<commandBufferSize;i++){
          Serial1.print(commandBuffer[i]);
          Serial1.print(" at ");
          Serial1.println(i);
        }
    
        //check byte cohesion
        boolean isBufferValid = true;
        if(commandBuffer[0] != 0xFF){
          isBufferValid = false;
          Serial1.println("invalid 0");
        }
        if(commandBuffer[1] != 0xFE){
          isBufferValid = false;
          Serial1.println("invalid 1");
        }
        if(commandBuffer[2] != 0xFD){
          isBufferValid = false;
          Serial1.println("invalid 2");
        }
        switch(commandBuffer[3]){
          case 0:
          if(commandBuffer[7] != NULL){
            isBufferValid = false;
            Serial1.println("NULL invalid");
          }
          break;
          case 1:
          if(commandBuffer[8] != NULL){
            isBufferValid = false;
            Serial1.println("NULL invalid");
          }
          break;
          case 2:
          if(commandBuffer[9] != NULL){
            isBufferValid = false;
           Serial1.println("NULL invalid");
          }
          break;
          default:
          isBufferValid = false;
          Serial1.println("invalid command");
          break;
        }
        
        if(isBufferValid == true){
          boolean isCheckValid = true;
          //checksum
          //
          //
          if(isCheckValid == true){
            //write commands to motor
            switch(commandBuffer[3]){
              case 0:
              motorStop(commandBuffer[4]);
              break;
              case 1:
              motorStep(commandBuffer[4],commandBuffer[5]);
              break;
              case 2:
              motorSpin(commandBuffer[4],commandBuffer[5],commandBuffer[6],commandBuffer[7]);
              break;
            }
          }else{
            Serial1.println("checksum invalid");
          }
        }else{
          Serial1.println("buffer invalid");
        }
      }else{
        Serial1.println("command invalid");
      }
    }else{
      Serial1.println("start not found");
    }
  }else{
    Serial1.println("waiting for data on rx");
  }
}
  
void motorStop(uint8_t channel){
  motor_t motor;
  motor.throttle = 0;
  switch(channel){
    case LEFT:
    motor.enable = ENA;
    break;
    case RIGHT: 
    motor.enable = ENB;
    break;
    default:
    Serial1.println("invalid channel");
    break;
  }
  analogWrite(motor.enable,motor.throttle);
}

void motorStep(uint8_t channel,uint8_t mode){
  motor_t motor;
  motor.throttle = STEP_PWM;
  motor.time = VS_GRADIENT/motor.throttle/1000;
  switch(channel){
    case LEFT:
    motor.enable = ENA;
    motor.gate1 = IN1;
    motor.gate2 = IN2;
    break;
    case RIGHT:
    motor.enable = ENB;
    motor.gate1 = IN3;
    motor.gate2 = IN4;
    break;
    default:
    Serial1.println("invalid channel");
    break;
  }
  switch(mode){
    case CW:
    digitalWrite(motor.gate1,HIGH);
    digitalWrite(motor.gate2,LOW);
    break;
    case CCW:
    digitalWrite(motor.gate1,LOW);
    digitalWrite(motor.gate2,HIGH);
    break;
    default:
    Serial1.println("invalid mode");
    break;
  }
  
  analogWrite(motor.enable,motor.throttle);
  delay(motor.time);
  analogWrite(motor.enable,STOP_PWM);
}

void motorSpin(uint8_t channel,uint8_t mode, uint8_t pulse,uint8_t time){
  motor_t motor;
  motor.time = time;
  motor.throttle = map(pulse,0,100,0,255);
  
  switch(channel){
    case LEFT:
    motor.enable = ENA;
    motor.gate1 = IN1;
    motor.gate2 = IN2;
    break;
    case RIGHT:
    motor.enable = ENB;
    motor.gate1 = IN3;
    motor.gate2 = IN4;
    break;
    default:
    Serial1.println("invalid channel");
    break;
  }
  
  analogWrite(motor.enable,motor.throttle);
  delay(motor.time);
  analogWrite(motor.enable,STOP_PWM);
}
