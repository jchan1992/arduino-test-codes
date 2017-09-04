//float send mega

typedef union{
 float num;
 byte bite[4];
} byteFloat;

byteFloat rec;
float rec1;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  rec.num = 0;
}

void loop(){
//  if(Serial1.available()>0){
//    for(int i = 0; i<4;i++){
//      rec.bite[i] = Serial1.read();
//    }
//  }
//  
rec1 = Serial1.parseFloat();
Serial.println(rec1);

//  Serial.println(rec.num);
}
