#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include "sensors.h"

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

using namespace nustars;

RH_RF95 rf95(RFM95_CS, RFM95_INT);

GPS* gps;
Altimeter* alt;
Accelerometer* acc;

uint8_t packetspam[250];

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Wire.begin();
  delay(1000);

  gps = new GPS();
  alt = new Altimeter();
  acc = new Accelerometer();

  /* Radio code
  Serial.println("Starting setup");
  pinMode(RFM95_RST, OUTPUT);

digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  pinMode(13, OUTPUT);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
 
  rf95.setTxPower(23, false);

  for (int i = 0; i < 250; i++) {
    packetspam[i] = 'A';
  }
  Serial.println("finished setup");
  */
}

void loop() {
  delay(50);
  gps->tick();
  alt->tick();
  acc->tick();
  Serial.println(gps->getSat());
  Serial.println(acc->getAcceleration(1));

  /* Radio code
  Serial.println("Transmitting");
  digitalWrite(13, HIGH);
  rf95.send(packetspam, 250);
  digitalWrite(13, LOW);
  //delay(1000);
  rf95.waitPacketSent();
  */
  
}
