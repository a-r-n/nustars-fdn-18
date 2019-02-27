#include "radio.h"

namespace nustars {
    Radio::Radio(int cs, int rst, int interrupt) : csPin(cs), rstPin(rst), intPin(interrupt) {
        rf95 = new RH_RF95(csPin, intPin);
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        delay(10);
        digitalWrite(rstPin, LOW);
        delay(10);
        digitalWrite(rstPin, HIGH);
        delay(10);

        while (!rf95->init()) {
            Serial.println("LoRa radio init failed");
            while (1);
        }

        if (!rf95->setFrequency(RF95_FREQ)) {
            Serial.println("setFrequency failed");
            while (1);
        }

        rf95->setTxPower(23, false);
    }

    void Radio::transmit(uint8_t* x, uint8_t length) {
        rf95->send(x, length);
    }
}
