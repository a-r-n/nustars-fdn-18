#include <SPI.h>
#include <Wire.h>
#include "sensors.h"
#include <Adafruit_NeoPixel.h>
#include "radio.h"

using namespace nustars;

GPS* gps;
Altimeter* alt;
IMU* imux;
ADXL* adxl;
Radio* radio;

Adafruit_NeoPixel led = Adafruit_NeoPixel(1, 8);

uint8_t packetBuffer[220];

union fusion_t {
    float f;
    int i;
    uint8_t b[sizeof(float) > sizeof(int) ? sizeof(float) : sizeof(int)];
};

fusion_t fusion;

void setup() {
    led.begin();
    setLED(255, 255, 0); //YELLOW: we are booting
    
    Serial.begin(9600);
    while (!Serial);
    Wire.begin();
    delay(1000);

    gps = new GPS();
    alt = new Altimeter();
    imux = new IMU();
    adxl = new ADXL(A1, A2, A4);
    radio = new Radio(A0, A5, A3);

    packetBuffer[0] = 'N';
    packetBuffer[1] = 'U';
    Serial.println("finished setup");
    setLED(80, 0, 120);
    
}

void loop() {
    //update the sensors
    gps->tick();
    alt->tick();
    imux->tick();
    adxl->tick();

    int bufferLocation = 2; //offset for packet identifying prefix


    //WARNING: No verification is being done to ensure data length. Keep it under byte limit please
    //package the current time
    fusion.i = millis();
    pack(fusion, packetBuffer, bufferLocation, sizeof(int));

    //package the data from the IMUx
    fusion.f = imux->getAcceleration(X_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getAcceleration(Y_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));
    fusion.f = imux->getAcceleration(Z_AXIS);
    pack(fusion, packetBuffer, bufferLocation, sizeof(float));

    //transmit the data
    radio->transmit(packetBuffer, bufferLocation + 1);

}

void setLED(uint8_t r, uint8_t g, uint8_t b) {
    led.setPixelColor(0, r, g, b);
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
