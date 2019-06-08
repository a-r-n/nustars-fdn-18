#ifndef _DEF_BNO
#define _DEF_BNO
#include <Adafruit_BNO055.h>
#endif

#ifndef  _DEF_BMP
#define _DEF_BMP
#include <Adafruit_BMP280.h>
#endif

#include <Adafruit_GPS.h> //TODO: plz add include guards
#include <TinyGPS++.h>

#define BMP_ADDR 0x77

namespace nustars {
    static const int X_AXIS = 0;
    static const int Y_AXIS = 1;
    static const int Z_AXIS = 2;

    class Sensor {
        virtual void tick();
    };

    /**
     * Adafruit BNO, the IMU
     */
    class IMU: public Sensor {
    private:
        Adafruit_BNO055 bno;
        float* orientation;
        float* acc;
        float* gyro;
    public:
        IMU();
        void tick() override;
        void reconnect();
        float getOrientation(int axis);
        float getAcceleration(int axis);
        float getGyro(int axis);
    };

    /**
     * Adafruit ADXL377, the analog accelerator
     */
     class ADXL: public Sensor {
      private:
        float x, y, z;
        int xPin, yPin, zPin;
      public:
        ADXL(int xPin, int yPin, int zPin);
        void tick() override;
        float getAcceleration(int axis);
     };

    /**
     * Adafruit BME, the altimeter
     */
    class Altimeter: public Sensor {
    private:
        Adafruit_BMP280 bme;
        float temp, pressure, alt, baseAlt;
        void setBaseAlt();
    public:
        Altimeter();
        void tick() override;
        float getTemp();
        float getPressure();
        float getAltitude();
    };
}
