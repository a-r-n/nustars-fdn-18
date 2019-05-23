#define PACKET_SIZE 10

int i = 0;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
}


void loop() {

  // put your main code here, to run repeatedly:

  i++;
  Serial.print("NU");

  int data1 = i*2;
  Serial.print(data1);
  
  int data2 = i*i;
  Serial.print(data2);
  
  if (i == 10) {
    Serial.end();
  }
}