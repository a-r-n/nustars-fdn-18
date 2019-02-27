#define PACKET_SIZE 10

 

 

void setup() {

  // put your setup code here, to run once:

 

  Serial.begin(9600);

 

}

 

 

void loop() {

  Serial.println("LOOP");

  // put your main code here, to run repeatedly:

  Serial.println("BEGIN");

  for (int i = 0; i < 10; i++) {

     Serial.print("NUx ");

 

    float data1 = 1.0/i;

    Serial.print(data1);

    Serial.print(" ");

    int data2 = i*i;

    Serial.print(data2);

    Serial.print(" ");

    Serial.println();

  }

  Serial.println("END");

  Serial.end();

}
