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

uint8_t packetspam[250];

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


    for (int i = 0; i < 250; i++) {
        packetspam[i] = 'A';
    }
    Serial.println("finished setup");
    setLED(80, 0, 120);
    
}

void loop() {
    //delay(200);
    gps->tick();
    alt->tick();
    imux->tick();
    adxl->tick();
    Serial.println(alt->getTemp());
    //Serial.println(gps->getSat());
    

}

void setLED(uint8_t r, uint8_t g, uint8_t b) {
    led.setPixelColor(0, r, g, b);
    led.show();
}
