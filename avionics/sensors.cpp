#include "sensors.h"

namespace nustars {

    /**********ACCELEROMETER
     ***********************
     **********************/

    /**
     * Initialize the BNO
     */
    Accelerometer::Accelerometer() {
        bno = Adafruit_BNO055(55); //I2C address, probably.
        orientation = new int[3];
        if (!bno.begin()) {
            Serial.print(
                    "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"); //TODO: Learn to throw an exception
            while (1);
        }
        bno.setExtCrystalUse(true);
    }

    /**
     * Update the orientation information
     */
    void Accelerometer::tick() {
        float collect[3];
        // The data will be very noisy, so we have to apply a moving average
        const int NUM_SAMPLES = 5;
        for (int i = 0; i < NUM_SAMPLES; i++) {
            sensors_event_t event;
            bno.getEvent(&event);
            collect[0] += event.orientation.x;
            collect[1] += event.orientation.y;
            collect[2] += event.orientation.z;
        }
        orientation[0] = (int)(collect[0] / NUM_SAMPLES);
        orientation[1] = (int)(collect[1] / NUM_SAMPLES);
        orientation[2] = (int)(collect[2] / NUM_SAMPLES);
    }

    /**
     * Get the current orientation
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the orientation on requested axis
     */
    int Accelerometer::getOrientation(int axis) {
        switch(axis) {
            case 0: return orientation[0];
            case 1: return orientation[1];
            case 2: return orientation[2];
            default: return -1; //TODO: Throw an exception
        }
    }

    /**
     * Get the current acceleration
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the acceleration on requested axis
     */
    int Accelerometer::getAcceleration(int axis) {
        imu::Vector<3> acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        switch(axis) {
            case 0: return acc.x();
            case 1: return acc.y();
            case 2: return acc.z();
            default: return -1; //TODO: Throw an exception
        }
    }


    /***********ALTIMETER
     ********************
     *******************/

    /**
     * Initialize the BME and perform starting tasks
     */
    Altimeter::Altimeter() {
        if (!bme.begin()) {
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            while (1);
        }
        setBaseAlt();
    }

    /**
     * Take several samples of the current altitude and record the average
     */
    void Altimeter::setBaseAlt() {
        const int NUM_BASE_SAMPLES = 10;
        //Set altitude at ground
        baseAlt = 0;
        for (int i = 0; i < NUM_BASE_SAMPLES; i++) {
            baseAlt += bme.readAltitude(1000);
        }
        baseAlt = baseAlt / NUM_BASE_SAMPLES; //average readings
    }

    /**
     * Update the current altitude, pressure, and temperature
     */
    void Altimeter::tick() {
        temp = bme.readTemperature();
        pressure = bme.readPressure();
        alt = bme.readAltitude(1000) - baseAlt;
    }

    //Altimeter getters

    //get temperature
    int Altimeter::getTemp() {
        return temp;
    }

    //get altitude
    int Altimeter::getAltitude() {
        return alt;
    }

    //get pressure
    int Altimeter::getPressure() {
        return pressure;
    }

    /**
     * Say hello to the GPS
     */
    GPS::GPS() {
        ada_gps = new Adafruit_GPS(&Serial1);
        lat = 0;
        lng = 0;
        alt = 0;
    }

    /**
     * Uses TinyGPS to read from GPS serial and encode information into usable variables
     */
    void GPS::tick() {
        while (Serial1.available()) //if die use >0
            tgps.encode(Serial1.read());
        lat = tgps.location.lat();
        lng = tgps.location.lng();
        alt = tgps.altitude.meters();
        numSat = tgps.satellites.value();
    }

    //ALL OF THE GETTERS
    float GPS::getAlt() {
        return alt;
    }

    float GPS::getLat() {
        return lat;
    }

    float GPS::getLng() {
        return lng;
    }

    int GPS::getSat() {
        return numSat;
    }
} //END NAMESPACE
