//NOTE: 255 bytes is the ABSOLUTE MAX packet size

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

char* printBuffer;
uint8_t* buf;

void setup() {
  printBuffer = (char*)malloc(255);
  buf = (uint8_t*)malloc(255);
  delay(5000);
  Serial.begin(9600);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!Serial);
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
  digitalWrite(13, HIGH);
  while (!rf95.available());
  uint8_t len = 255;
  rf95.recv(buf, &len);
  if (Serial.availableForWrite()) {
    Serial.write(buf, len);
  }
  digitalWrite(13, LOW);
}
