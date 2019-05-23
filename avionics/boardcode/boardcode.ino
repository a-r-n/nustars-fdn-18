// setting pins for raven 0A, etc
int r0A = 10; 
int r0B = 11; 
int r1A = 12; 
int r1B = 13; 
// variables to store values for each pin
int val0A = 0; 
int val0B = 0;
int val1A = 0; 
int val1B = 0;

void setup() {
  pinMode(r0A, INPUT); 
  pinMode(r0B, INPUT); 
  pinMode(r1A, INPUT); 
  pinMode(r1B, INPUT); 
}

void loop() {
  val0A = digitalRead(r0A); 
  val0B = digitalRead(r0B); 
  val1A = digitalRead(r1A); 
  val1B = digitalRead(r1B); 
}
