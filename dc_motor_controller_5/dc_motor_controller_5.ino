//define pins for h bridge
#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

//define constants
#define VS_GRADIENT 11
#define RX_SIZE 256
#define BAUD_RATE 9600
#define STEP_PWM 120
#define STOP_PWM 0
#define LEFT 1
#define RIGHT 2
#define CW 1
#define CCW 2
#define MIN_COMMAND_LENGTH 7
#define COMMAND_BUFFER_0 7
#define COMMAND_BUFFER_1 8
#define COMMAND_BUFFER_2 10

//define motor
typedef struct motor_t{
    uint8_t enable;
    uint8_t gate1;
    uint8_t gate2;
    uint8_t throttle;
    uint8_t time;
}motor_t;

//motor functions
void motorStop(uint8_t);
void motorStep(uint8_t,uint8_t);
void motorSpin(uint8_t,uint8_t,uint8_t);

//check booleans
boolean isRxLengthValid = false;
boolean isStartFound = false;
boolean isCommandValid = false;
boolean isBufferValid = false;
boolean isCheckValid = false;
boolean isCommandLengthValid = false;

//buffers
unsigned char rxBuffer[RX_SIZE];
int8_t rxSize;
uint8_t rxCount;
uint8_t commandSize;
uint8_t commandCount;

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
  Serial1.println("waiting for incoming data");
}

void serialEvent1(){
  //initialise checks
  Serial1.println("initialising checks");
  isRxLengthValid = false;
  isStartFound = false;
  isCommandValid = false;
  isBufferValid = false;
  isCheckValid = false;
  
  //initialise iterators
  Serial1.println("initialising iterators");
  rxCount = 0;
  commandCount = 0;
  rxSize = 0;
  //wait for data on rx
  if(Serial1.available()>0){
    //read data into a buffer
    rxSize = Serial1.available();
    Serial1.println("available data on rx");    
    Serial1.println(rxSize);
    for(rxCount = 0; rxCount<rxSize;rxCount++){
      rxBuffer[rxCount] = Serial1.read();
      Serial1.print(rxBuffer[rxCount]);
      Serial1.print(" at ");
      Serial1.println(rxCount);
    }
  }else{
    Serial1.println("no incoming data");
    loop();
  }
  
  //check buffer length 
  if(rxSize >= MIN_COMMAND_LENGTH){
    isRxLengthValid = true;
    Serial1.println("rx length valid");
  }
  if(isRxLengthValid == true){
    Serial1.println("finding start bytes");
    //find start bytes
    rxCount = 0;
    while(isStartFound == false && rxCount < rxSize){
//      Serial1.print(isStartFound);
//      Serial1.print("   ");
//      Serial1.print(rxCount);
//      Serial1.print("   ");
//      Serial1.println(rxSize);
      
      if((rxBuffer[rxCount] != 0xFF) || (rxBuffer[rxCount+1] !=0xFE) || (rxBuffer[rxCount+2] != 0xFD)){
        Serial1.print(rxBuffer[rxCount]);
        Serial1.print("\t");
        Serial1.print(rxBuffer[rxCount+1]);
        Serial1.print("\t");
        Serial1.println(rxBuffer[rxCount+2]);
        rxCount++;
      }else{
        isStartFound = true;
        Serial1.print("found start bytes  ");
        Serial1.print(rxBuffer[rxCount]);
        Serial1.print(" at ");
        Serial1.println(rxCount);
      }
    }
    if(isStartFound == true){
      //check command  
      if(rxBuffer[rxCount+3]>=0 && rxBuffer[rxCount+3] <=2){
        isCommandValid = true;
        Serial1.print("command is valid  ");
        Serial1.print(rxBuffer[rxCount+3]);
        Serial1.print(" at ");
        Serial1.println(rxCount+3);
      }
      if(isCommandValid ==true){
        //check command length;
        uint8_t commandBufferSize = 0;
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
        }
        Serial1.print("command buffer size ");
        Serial1.print(commandBufferSize);
        Serial1.print("   ");
        Serial1.print(rxCount+commandBufferSize-1);
        Serial1.print("   ");
        Serial1.println(rxBuffer[rxCount+commandBufferSize-1]);
        if(rxBuffer[rxCount+commandBufferSize-1] == 0xFC){
          isCommandLengthValid = true;
        }
        if(isCommandLengthValid == true){  
          Serial1.println("command length is valid");
          //copy command into command buffer
          unsigned char commandBuffer[commandBufferSize];     
          for(commandCount = 0;commandCount<commandBufferSize;commandCount++){
            commandBuffer[commandCount] = rxBuffer[rxCount];
            Serial1.print(commandBuffer[commandCount]);
            Serial1.print(" at ");
            Serial1.println(commandCount);
            rxCount++;
          }
          //check buffer order
          if(commandBuffer[0] == 0xFF && commandBuffer[1] == 0xFE && commandBuffer[2] == 0xFD &&commandBuffer[commandBufferSize-1] == 0xFC){
            isBufferValid = true;
            Serial1.println("buffer structure correct");
          }
          if(isBufferValid == true){
            //checksum...
            //if(1){
              isCheckValid = true;
            //}          
            if(isCheckValid == true){
              Serial1.println("checksum correct");
              switch(commandBuffer[3]){
                case 0:
                motorStop(commandBuffer[4]);
                Serial1.println("excuting motor stop");
                loop();
                //Serial1.end();
                break;
                case 1:
                motorStep(commandBuffer[4],commandBuffer[5]);
                Serial1.println("executing motor step");
                loop();
                //Serial1.end();
                break;
                case 2:
                motorSpin(commandBuffer[4],commandBuffer[5],commandBuffer[6],commandBuffer[7]);
                Serial1.println("executing motor spin");
                loop();
                //Serial1.end();
                break;
              }  
            }else{
              Serial1.println("check sum invalid");
              //Serial1.end();
              loop();
              
            }
          }else{
            Serial1.println("buffer invalid");
            //Serial1.end();
            loop();
          }
        }else{
          Serial1.println("command length invalid");
          //Serial1.end();
          loop();
        }
      }else{
        Serial1.println("command invalid");
        //Serial1.end();
        loop();
      }
    }else{
      Serial1.println("start not found");
      //Serial1.end();
      loop();
    }
  }else{
    Serial1.println("buffer too short");
    //Serial1.end();
    //loop();
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
  motor.time = 50;
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
