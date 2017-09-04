const float Vmin = 0;
const float Vmax = 9.88;
const int beforePin = A0;
const int afterPin = A1;
const int Resistance = 1;
int Abefore = 0;
int Aafter = 0;
float Vbefore = 0;
float Vafter = 0;
float Vdiff = 0;
float Current = 0;

void setup(){
  Serial1.begin(9600);
}

void loop(){
  Abefore = analogRead(beforePin);
  Aafter = analogRead(afterPin);
  Vbefore = map(Abefore,0,1023,Vmin,Vmax);
  Vafter = map(Aafter,0,1023,Vmin,Vmax);
  Vdiff = abs(Vbefore-Vafter);
  Current = Vdiff/Resistance;
  
  Serial1.print(Vbefore);
  Serial1.print("\t");
  Serial1.print(Vafter);
  Serial1.print("\t");
  Serial1.print(Vdiff);
  Serial1.print("\t");
  Serial1.println(Current);
  delay(100);
}
