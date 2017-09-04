char tx[10];

void setup(){
  Serial1.begin(9600);
}

void loop(){
  if(Serial1.available()>0){
    Serial1.readBytes(tx,10);
  }
  for(byte i = 0;i<10;i++){
  Serial1.write(tx[i]);
  }
}
