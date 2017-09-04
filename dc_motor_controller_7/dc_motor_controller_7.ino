//define pins for h bridge
#define ENA  6
#define ENB  5
#define IN1  4
#define IN2  7
#define IN3  3
#define IN4  2

//define pins for analog
#define LEFT_CURRENT_PIN A0
#define RIGHT_CURRENT_PIN A1

//buffer constant values
#define START_1 0xFF
#define START_2 0xFE
#define START_3 0xFD
#define END_1 0xFC
#define END_2 0xFB

//define buffer lengths
#define RX_SIZE 256
#define MIN_COMMAND_LENGTH 8
#define COMMAND_BUFFER_0 8
#define COMMAND_BUFFER_1 9
#define COMMAND_BUFFER_2 11
#define COMMAND_BUFFER_3 10

#define BAUD_RATE 9600
#define STEP_PWM 50
#define STOP_PWM 0

#define LEFT 0
#define RIGHT 1

#define CW 0
#define CCW 1

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
void motorSpin(uint8_t,uint8_t,uint8_t,uint8_t);
void motorFreeSpin(uint8_t,uint8_t,uint8_t);

//check booleans
boolean isRxLengthValid = false;
boolean isStartFound = false;
boolean isEndFound = false;
boolean isCommandValid = false;
boolean isStructureValid = false;
boolean isCheckValid = false;

//buffer data
unsigned char rxBuffer[RX_SIZE];
uint8_t rxSize;
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
  Serial1.println(analogRead(LEFT_CURRENT_PIN));
  Serial1.println(analogRead(RIGHT_CURRENT_PIN));
}

void serialEvent1(){
  //initialise checks
  Serial1.println("initialising checks");
  isRxLengthValid = false;
  isStartFound = false;
  isCommandValid = false;
  isEndFound = false;
  isStructureValid = false;
  isCheckValid = false;
  
  //initialise iterators
  Serial1.println("initialising iterators");
  rxCount = 0;
  commandCount = 0;
  rxSize = 0;
  commandSize = 0;
  
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
      if((rxBuffer[rxCount] != START_1) || (rxBuffer[rxCount+1] !=START_2) || (rxBuffer[rxCount+2] != START_3)){
        Serial1.print(rxBuffer[rxCount]);
        Serial1.print("\t");
        Serial1.print(rxBuffer[rxCount+1]);
        Serial1.print("\t");
        Serial1.println(rxBuffer[rxCount+2]);
        rxCount++;
      }else{
        isStartFound = true;
      }
    }
    if(isStartFound == true){
        Serial1.print("found start bytes  ");
        Serial1.print(rxBuffer[rxCount]);
        Serial1.print(" at b");
        Serial1.println(rxCount);
      //check command  
      Serial1.println("checking command");
      if(rxBuffer[rxCount+3]>=0 && rxBuffer[rxCount+3] <=3){
        isCommandValid = true;
      }
      if(isCommandValid ==true){
        Serial1.print("command is valid command:  ");
        Serial1.print(rxBuffer[rxCount+3]);
        Serial1.print(" at b");
        Serial1.println(rxCount+3);
        //set command length;
        switch(rxBuffer[rxCount+3]){
          case 0:
          commandSize = COMMAND_BUFFER_0;
          break;
          case 1:
          commandSize = COMMAND_BUFFER_1;
          break;
          case 2:
          commandSize = COMMAND_BUFFER_2;
          break;
          case 3:
          commandSize = COMMAND_BUFFER_3;
        }
        Serial1.print("command buffer size ");
        Serial1.println(commandSize);
        Serial1.println("finding end bytes");
        if(rxBuffer[rxCount+commandSize-2] == END_1 && rxBuffer[rxCount + commandSize-1] == END_2){
          isEndFound = true;
        }else{
          Serial1.print(rxBuffer[rxCount+commandSize-2]);
          Serial1.print("   ");
          Serial1.println(rxBuffer[rxCount + commandSize-1]);
        }
        if(isEndFound == true){  
          Serial1.println("end found");
          Serial1.print(rxBuffer[rxCount+commandSize-2]);
          Serial1.print(" at b");
          Serial1.println(rxCount+commandSize-2);
          //copy command into command buffer
          Serial1.println("copying command to command buffer");
          unsigned char commandBuffer[commandSize];     
          for(commandCount = 0;commandCount<commandSize;commandCount++){
            commandBuffer[commandCount] = rxBuffer[rxCount];
            Serial1.print(commandBuffer[commandCount]);
            Serial1.print(" at ");
            Serial1.println(commandCount);
            rxCount++;
          }
          //check buffer structure
          Serial1.println("checking buffer structure");
          if(commandBuffer[0] == START_1 && commandBuffer[1] == START_2 && commandBuffer[2] == START_3 && commandBuffer[commandSize-2] == END_1 && commandBuffer[commandSize-1] == END_2 ){
            isStructureValid = true;
          }
          if(isStructureValid == true){
            Serial1.println("buffer structure correct");
            //checksum...
            Serial1.println("checking checksum");
            Serial1.print("checksum should be ");
            Serial1.println(commandBuffer[commandSize-3]);
            uint8_t checksum = 0;
            unsigned int sum = 0;
            for(commandCount = 0; commandCount<commandSize-3;commandCount++){
              sum = sum + commandBuffer[commandCount];
            }
            Serial1.print("sum is");
            Serial1.println(sum);
            checksum = sum & 0xFF ;
            Serial1.print("checksum is");
            Serial1.println(checksum);
            if(checksum == commandBuffer[commandSize-3]){
              isCheckValid = true;
            }          
            if(isCheckValid == true){
              Serial1.println("checksum correct");
              switch(commandBuffer[3]){
                case 0:
                motorStop(commandBuffer[4]);
                Serial1.println("excuting motor stop");
                //loop();
                //Serial1.end();
                break;
                case 1:
                motorStep(commandBuffer[4],commandBuffer[5]);
                Serial1.println("executing motor step");
                //loop();
                //Serial1.end();
                break;
                case 2:
                motorSpin(commandBuffer[4],commandBuffer[5],commandBuffer[6],commandBuffer[7]);
                Serial1.println("executing motor spin");
                //loop();
                //Serial1.end();
                break;
                case 3:
                motorFreeSpin(commandBuffer[4],commandBuffer[5],commandBuffer[6]);
                break;
              }  
            }else{
              Serial1.println("check sum invalid");
              //Serial1.end();
              //loop();
            }
          }else{
            Serial1.println("command structure invalid");
            //Serial1.end();
            //loop();
          }
        }else{
          Serial1.println("end not found");
          //Serial1.end();
          //loop();
        }
      }else{
        Serial1.println("command invalid");
        //Serial1.end();
        //loop();
      }
    }else{
      Serial1.println("start not found");
      //Serial1.end();
      //loop();
    }
  }else{
    Serial1.println("buffer too short");
    //Serial1.end();
    //loop();
  }
}

void motorStop(uint8_t channel){
  motor_t motor;
  motor.throttle = STOP_PWM;
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
  motor.time = 100;
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
  motor_t motor1;
  motor_t motor2;
  motor1.time = time;
  motor2.time = time;
  motor1.throttle = map(pulse,0,100,0,255);
  motor2.throttle = motor1.throttle;
  switch(channel){
    case LEFT:
    motor1.enable = ENA;
    motor1.gate1 = IN1;
    motor1.gate2 = IN2;
    motor2.enable = ENB;
    motor2.gate1 = IN3;
    motor2.gate2 = IN4;
    break;
    case RIGHT:
    motor1.enable = ENB;
    motor1.gate1 = IN3;
    motor1.gate2 = IN4;
    motor2.enable = ENA;
    motor2.gate1 = IN1;
    motor2.gate1 = IN2;
    break;
    default:
    Serial1.println("invalid channel");
    break;
  }
    switch(mode){
    case CW:
    digitalWrite(motor1.gate1,HIGH);
    digitalWrite(motor1.gate2,LOW);
    digitalWrite(motor2.gate1,HIGH);
    digitalWrite(motor2.gate2,LOW);
    break;
    case CCW:
    digitalWrite(motor1.gate1,LOW);
    digitalWrite(motor1.gate2,HIGH);
    digitalWrite(motor2.gate1,LOW);
    digitalWrite(motor2.gate2,LOW);
    break;
    default:
    Serial1.println("invalid mode");
    break;
  }
  
  analogWrite(motor1.enable,motor1.throttle);
  analogWrite(motor2.enable,motor2.throttle);
  delay(motor1.time);
  analogWrite(motor1.enable,STOP_PWM);
  analogWrite(motor2.enable,STOP_PWM):
}

void motorFreeSpin(uint8_t channel,uint8_t mode,uint8_t pulse){
  motor_t motor1;
  motor_t motor2;
  motor1.throttle = map(pulse,0,100,0,255);
  motor2.throttle = motor1.throttle;
  switch(channel){
    case LEFT:
    motor1.enable = ENA;
    motor1.gate1 = IN1;
    motor1.gate2 = IN2;
    motor2.enable = ENB;
    motor2.gate1 = IN1;
    motor2.gate2 = IN2;
    break;
    case RIGHT:
    motor1.enable = ENB;
    motor1.gate1 = IN3;
    motor1.gate2 = IN4;
    motor2.enable = ENA;
    motor2.gate1 = IN1;
    motor2.gate2 = IN2;
    break;
    default:
    Serial1.println("invalid channel");
    break;
  }
    switch(mode){
    case CW:
    digitalWrite(motor1.gate1,HIGH);
    digitalWrite(motor1.gate2,LOW);
    digitalWrite(motor2.gate1,HIGH);
    digitalWrite(motor2.gate2, LOW);
    break;
    case CCW:
    digitalWrite(motor1.gate1,LOW);
    digitalWrite(motor1.gate2,HIGH);
    digitalWrite(motor2.gate1,LOW);
    digitalWrite(motor2.gate2,HIGH);
    break;
    default:
    Serial1.println("invalid mode");
    break;
  }
  analogWrite(motor1.enable,motor1.throttle);
  analogWrite(motor1.enable,motor2.throttle);
  
}
  
