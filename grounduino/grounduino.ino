//LIFTED FROM COMPONENT EXAMPLES -- needs experimentation and more work generally obv

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

uint8_t buffer[1000] = {0};

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
 
  rf95.setTxPower(23, false);
}

int numbytes = 0;

void loop() {
  while (!rf95.available());
  
    uint8_t buf[100];
    uint8_t len = sizeof(buf);
    
    rf95.recv(buf, &len);
    for (int i = 0; i < 100; i++) {
      if (buffer[i] != 0)
        numbytes++;
    }
    Serial.println(buffer[0]);
}
