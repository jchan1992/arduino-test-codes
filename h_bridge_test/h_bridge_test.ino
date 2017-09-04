const int ENA = 6;
const int ENB = 5;
const int IN1 = 4;
const int IN2 = 7;
const int IN3 = 3;
const int IN4 = 2;

void motorA(int mode,int power);
void motorB(int mode,int power);
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
    debug();
    motorA(1, 100);  //have motor A turn clockwise at 15% speed;  //let motor A run for 5 seconds
    motorB(1, 100);  //have motor B turn clockwise at 15% speed
    delay(100);
    motorA(2, 100);  //brake motor A with 100% braking power
    motorB(2, 100);  
    delay(100);

}
  
void motorA(int mode, int percent){
  int duty = map(percent,0,100,0,255);
  
  switch(mode){
    case 0: 
    digitalWrite(ENA,LOW);
    break;
    case 1:
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    analogWrite(ENA,duty);
    break;
    case 3:
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    analogWrite(ENA,duty);
    case 4:
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    analogWrite(ENA,duty);
    break;
  }
}

void motorB(int mode, int percent){
  int duty = map(percent,0,100,0,255);
  
  switch(mode){
    case 0:
    digitalWrite(ENB,LOW);
    break;
    case 1:
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
    analogWrite(ENB,duty);
    case 2:
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
    analogWrite(ENB,duty);
    break;
    case 3:
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    analogWrite(ENB,duty);
  }
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
