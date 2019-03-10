#include <SPI.h>
#include <Wire.h>
#include "sensors.h"
#include <Adafruit_NeoPixel.h>
#include "radio.h"
#include <Servo.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>

// FLASH DEFS TEMPORARY NO TIME

#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV  // Flash chip type.
                                              // If you change this be
                                              // sure to change the fatfs
                                              // object type below to match.

#if (SPI_INTERFACES_COUNT == 1)
  #define FLASH_SS       SS                    // Flash chip SS pin.
  #define FLASH_SPI_PORT SPI                   // What SPI port is Flash on?
#else
  #define FLASH_SS       SS1                    // Flash chip SS pin.
  #define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
#endif


// defining raven pins
#define R0A 10 
#define R0B 11 
#define R1A 12 
#define R1B 13

#define TSKIP_FACTOR 5 //how many reads to skip before transmitting (greatly reduce loop time) [temporary] TODO: implement this with interrupts
#define WSKIP_FACTOR 50 //how many reads to skip before flash writing (save loop time and flash space). Currently rated for approximately 5-10 hours of logging...
#define CSKIP_FACTOR 2 //unused


// setting debug stuff
#define DEBUG false
int divisor = 1; 

//TEMPORARY
Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);     // Use hardware SPI

// Alternatively you can define and use non-SPI pins!
// Adafruit_SPIFlash flash(FLASH_SCK, FLASH_MISO, FLASH_MOSI, FLASH_SS);

Adafruit_M0_Express_CircuitPython fatfs(flash);
//END TEMPORARY

// variables to store values for each pin
int val0a = 0; 
int val0b = 0;
int val1a = 0; 
int val1b = 0;

int tskip_step = 0;
int wskip_step = 0;

using namespace nustars;

GPS* gps;
Altimeter* alt;
IMU* imux;
ADXL* adxl;
Radio* radio;

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, 8);

uint8_t packetBuffer[220];
uint8_t writeBuffer[4096];
int wbPos = 0;

char* tag = "NUx ";
int tagLength = 4;

char* fn;

union fusion_t {
    float f;
    int i;
    long l;
    uint8_t b[sizeof(long)];
};

fusion_t fusion;

void setup() {
    if(DEBUG){
      divisor = 4; 
    }
  
    led.begin();
    setLED(255/divisor, 0, 0); //RED: we are booting
    
    Serial.begin(9600);

    //wait for serial connection if we are in debug mode
    if(DEBUG)
    {
      while (!Serial);
    }
    Wire.begin();
    delay(1000);

    //TEMPORARY WRITING
    // Initialize flash library and check its chip ID.
  if (!flash.begin(FLASH_TYPE)) {
    Serial.println("Error, failed to initialize flash chip!");
    while(1);
  }
  Serial.print("Flash chip JEDEC ID: 0x"); Serial.println(flash.GetJEDECID(), HEX);

  // First call begin to mount the filesystem.  Check that it returns true
  // to make sure the filesystem was mounted.
  if (!fatfs.begin()) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    while(1);
  }
  Serial.println("Mounted filesystem!");

    int dataID = 0;
    fn = (char*)calloc(20, 1);
    do {
      sprintf(fn, "data%d.hex\0", dataID);
      Serial.println("finding:");
      Serial.println(fn);
      dataID++;
    } while (fatfs.exists(fn));

    Serial.print("found file that doesn't exist yet: ");
    Serial.println(fn);

  //END TEMPORARY
  
    gps = new GPS();
    Serial.println("Q: Where");
    alt = new Altimeter();
    Serial.println("are");
    imux = new IMU();
    Serial.println("we");
    adxl = new ADXL(A1, A2, A4);
    Serial.println("?");
    radio = new Radio(A0, A5, A3);

    Serial.println("A: Writing the tag!");
    for (int i = 0; i < tagLength; i++) {
        packetBuffer[i] = tag[i];
    }
    
    
    setLED(80/divisor, 0, 120/divisor); //PURPLE: everything okay

    // Raven detection pins
    pinMode(R0A, INPUT); 
    pinMode(R0B, INPUT); 
    pinMode(R1A, INPUT); 
    pinMode(R1B, INPUT); 



    
    Serial.println("finished setup");
}

void loop() {
    //update the sensors
    gps->tick();
    alt->tick();
    imux->tick();
    adxl->tick();

    int bufferLocation = tagLength; //offset for packet identifying prefix


    //WARNING: No verification is being done to ensure data length. Keep it under byte limit please
    //package the current time
    fusion.l = millis();
    pack(fusion, packetBuffer, bufferLocation, sizeof(long));

    //package the data from the IMUx
    fusion.f = imux->getAcceleration(X_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getAcceleration(Y_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getAcceleration(Z_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));

    //IMUx rotation
    fusion.f = imux->getOrientation(X_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getOrientation(Y_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getOrientation(Z_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));

    //ADXL acceleration
    fusion.f = adxl->getAcceleration(X_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = adxl->getAcceleration(Y_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = adxl->getAcceleration(Z_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));

    //BMP
    fusion.f = alt->getPressure();
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = alt->getTemp();
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));

    //GPS
    fusion.f = gps->getLat();
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = gps->getLng();
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = gps->getAlt();
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.i = gps->getSat();
    pack(fusion, packetBuffer, bufferLocation, sizeof(int));

    //Raven binary
    uint8_t raven = 0;
    val0a = digitalRead(R0A) == LOW; 
    val0b = digitalRead(R0B) == LOW; 
    val1a = digitalRead(R1A) == LOW; 
    val1b = digitalRead(R1B) == LOW;

    raven |= val0a;
    raven |= val0b << 1;
    raven |= val1a << 2;
    raven |= val1b << 3;

    fusion.b[0] = raven;
    pack(fusion, packetBuffer, bufferLocation, sizeof(uint8_t));

    Serial.println(bufferLocation + 1);

    
    /* //test the packing
    for (int i = 0; i < bufferLocation + 1; i++) {
      Serial.print(packetBuffer[i]);
      Serial.print(" ");
      if (i%4 == 3) {
        Serial.println();
      }
    }
    for (int i = 4; i < 8; i++) {
      fusion.b[i-4] = packetBuffer[i];
    }
    */
    
    //Serial.println(fusion.f);
    //transmit the data
    if (!(tskip_step % TSKIP_FACTOR)) {
      radio->transmit(packetBuffer, bufferLocation + 1);
      for (int i = 0; i < bufferLocation; i++) {
        writeBuffer[wbPos + i] = packetBuffer[i];
      }
    }
      
    tskip_step++;

    
    if (!(wskip_step % WSKIP_FACTOR)) {
      File writeFile = fatfs.open(fn, FILE_WRITE);
      if (!writeFile && DEBUG) {
        Serial.println("Error, failed to open file for writing!");
        while(1);
      } else if (writeFile) {
        writeFile.write(packetBuffer, bufferLocation + 1);
        Serial.println("wrote");
        writeFile.close();
        wbPos = 0;
      } else {
        setLED(255/divisor, 255/divisor, 0);
      }
    }
    wskip_step++;

}

void setLED(uint8_t r, uint8_t g, uint8_t b) {
    led.setPixelColor(0, r/divisor, g/divisor, b/divisor);
    led.show();
}

/**
 * Packs fusion data into a buffer, incrementing index by size
 * @param fuz fusion_t to pack
 * @param buff buffer to place the data into
 * @param index starting index of buff; incremented by size upon return
 * @param size how many bytes of data to pack
 */
void pack(fusion_t fuz, uint8_t* buff, int &index, uint8_t size) {
    for (int i = 0; i < size; i++) {
        buff[index++] = fuz.b[i];
    }
}

/* //TODO: Implement this. Disabled to get things working for launch.
void raven(){
    // read the pin
    val0a = digitalRead(r0a); 
    val0b = digitalRead(r0b); 
    val1a = digitalRead(r1a); 
    val1b = digitalRead(r1b);
}

*/
