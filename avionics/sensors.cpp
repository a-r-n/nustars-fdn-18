#include "sensors.h"

namespace nustars {

    /**********IMU
     ***********************
     **********************/

    /**
     * Initialize the BNO
     */
    IMU::IMU() {
        bno = Adafruit_BNO055(); //I2C address, probably.
        orientation = new float[3];
        gyro = new float[3];
        acc = new float[3];
        if (!bno.begin()) {
            Serial.print(
                    "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"); //TODO: Learn to throw an exception
            while (1);
        }
        bno.setExtCrystalUse(true);
        //Select page ID 1 to write to the right register
    }

    /**
     * Update the orientation information
     */
    void IMU::tick() {
        imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
        orientation[0] = euler.x();
        orientation[1] = euler.y();
        orientation[2] = euler.z();
        euler = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        gyro[0] = euler.x();
        gyro[1] = euler.y();
        gyro[2] = euler.z();
        euler = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        acc[0] = euler.x();
        acc[1] = euler.y();
        acc[2] = euler.z();
    }

    /**
     * Reconnect the BNO if it is lost
     */
    void IMU::reconnect() {
        bno.begin();
    }

    /**
     * Get the current orientation
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the orientation on requested axis
     */
    float IMU::getOrientation(int axis) {
        return orientation[axis];
    }

    float IMU::getGyro(int axis) {
        return gyro[axis];
    }

    /**
     * Get the current acceleration
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the acceleration on requested axis
     */
    float IMU::getAcceleration(int axis) {
        return acc[axis];
    }


    /***********ADXL
     ********************
     *******************/
    ADXL::ADXL(int xPin, int yPin, int zPin) {
        this->xPin = xPin;
        this->yPin = yPin;
        this->zPin = zPin;
    }

    void ADXL::tick() {
        //unimplemented. We get the values just in time as they are one at a time anyway and we probably only want one
    }

    float ADXL::getAcceleration(int axis) {
        float rawData;
        switch (axis) {
            case X_AXIS:
                rawData = analogRead(xPin);
            case Y_AXIS:
                rawData = analogRead(yPin);
            case Z_AXIS:
                rawData = analogRead(zPin);
        }
        return (float)(rawData) * 400.0 / 1023.0 - 200; //this may not be completely accurate, but we are doing this to get mvp by the weekend
    }



    /***********ALTIMETER
     ********************
     *******************/

    /**
     * Initialize the BME and perform starting tasks
     */
    Altimeter::Altimeter() {
        if (!bme.begin(BMP_ADDR)) {
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            while (1);
        }
        //bme.begin(0x76);
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
    float Altimeter::getTemp() {
        return temp;
    }

    //get altitude
    float Altimeter::getAltitude() {
        return alt;
    }

    //get pressure
    float Altimeter::getPressure() {
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
