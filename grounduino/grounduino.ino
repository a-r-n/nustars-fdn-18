//NOTE: 255 bytes is the ABSOLUTE MAX packet size

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

#define DEBUG 1
#define PACKET_SIZE 70

RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint8_t* recvBuf;
uint8_t* queue;
uint8_t head, tail;
int sz;

/**
 * Pushes bytes starting at x onto the queue
 */
uint8_t pushq(uint8_t* x, uint8_t len) {
  //abort if the queue cannot accomodate this new information
  
  if (255 - sz < (int)len) {
    return 0;
  }
  //copy the data here
  for (int i = 0; i < len; i++) {;
    queue[tail++] = x[i];
    sz++;
  }
  return 1;
}

/**
 * return the first byte in the queue
 * only call this after checking queue sz
 */
uint8_t popq() {
  uint8_t x = queue[head];
  head++;
  sz--;
  return x;
}

void setup() {
  head = 0;
  tail = 0;
  sz = 0;
  Serial.begin(115200);
  while (!Serial);
  recvBuf = (uint8_t*)malloc(256);
  queue = (uint8_t*)malloc(256); //abuse overflow for fast indexing by making it this size

  if (!recvBuf || !queue) {
    Serial.println("malloc failed!");
    while (1);
  }

  //BEGIN SETTING UP THE RADIO
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
  //Serial.println("Setup complete");
  rf95.setTxPower(23, false);
  //END SETTING UP THE RADIO
  
}

void loop() {
  digitalWrite(13, HIGH);
  while (!rf95.available());
  uint8_t len = 255;
  rf95.recv(recvBuf, &len);
  //Serial.write(recvBuf, PACKET_SIZE-5);
  pushq(recvBuf, len);

  uint8_t done = 0;
  if (sz >= 2*PACKET_SIZE) {
    while (!done || sz < PACKET_SIZE) {
      for (uint8_t x = 0; x != 'N'; x = popq());
      if (popq() == 'U' && popq() == 'x') {
        popq(); //kill the space part of flag
        done = 1;
      }
    }
    if (done) {
      uint8_t checksum = 0;//'N' ^ 'U' ^ 'x' ^ ' ';
      uint8_t buf[255];
      for (int i = 0; i < PACKET_SIZE-5; i++) {
        uint8_t x = popq();
        checksum ^= x;
        buf[i] = x;
      }
      if (checksum == popq()) {
        Serial.write("NUx ", 4);
        Serial.write(buf, PACKET_SIZE-5);
      } else {
        //Serial.write(buf, PACKET_SIZE-5);
      }
    }
    
  }
  /*
  if (queueReadPos + 3*PACKET_SIZE < queueReadPos) {
    Serial.println("Got A");
    uint8_t checksum = 0;
    while (queue[queueReadPos] != 'N' || queue[queueReadPos+1] != 'U' || queue[queueReadPos+2] != 'x')
      queueReadPos++;
    if (Serial.availableForWrite()) {
      Serial.write(queue+queueReadPos, PACKET_SIZE);
    }
  }
  */
  digitalWrite(13, LOW);
}
