#include "PID.h"
#include "Arduino.h"

namespace nustars {
    /**
     *
     */
    PID::PID() {
        desiredX = 0; //the desired rotation
        modX = 0; //enable values for position outside the bounds [0, 360]
        previousError = 0; //the previous (x - desiredX); (note: x=(modX + sensor_x)
        previousX = 0; //previous sensor_x
        accumulatedError = 0; //questionable cumulative integration
        P = 0;
        I = 0;
        D = 0;
    }

    /**
     * Updates PID value information and sends appropriate signals to the motor.
     * @param sensor_x The current value of the X rotation in degrees
     */
    void PID::tick(int sensor_x) {

        int dx = previousX - sensor_x;
        /*//handles going past the 0 <-> 359 bound
         * We are not using this because the rocket will need to do a full rotation to account for
         * each accumulated rotation since launch
        if (dx >= 180) modX += 360;
        else if (dx <= -180) modX -= 360;
         */

        modX = (sensor_x + 180) % 360;
        modDesire = (desiredX + 180) % 360;
        previousX = sensor_x;
        currentError = modX - modDesire;


        if (accumulatedError > ERROR_CAP) { //limit integrating effects
            accumulatedError = ERROR_CAP;
        } else if (accumulatedError < -ERROR_CAP) {
            accumulatedError = -ERROR_CAP;
        }
        int derivError = previousError - currentError; //shifty differentiation :)

        //PID MAGIC
        double pidFactor = P * currentError + D * derivError + I * accumulatedError;
        accumulatedError += currentError; //shifty integration :)

        int analogOut;
        //output is limited to [-255, 255]
        if (pidFactor > 255) {
            analogOut = 255;
        } else if (pidFactor < -255) {
            analogOut = -255;
        } else {
            analogOut = (int)pidFactor;
        }

        //Serial.printf("Aout: %3d; cE: %d; SeX: %d\n", analogOut, currentError, sensor_x + modX);

        //handle the LEDs based on the actual error, not the pid value
        //TODO: Make this depend on global in main_code
        if (currentError < ZERO_TOLERANCE && currentError > ZERO_TOLERANCE) {
            digitalWrite(24, LOW);
            digitalWrite(25, HIGH);
            digitalWrite(26, LOW);
        } else if (currentError > 0) {
            digitalWrite(24, HIGH);
            digitalWrite(25, LOW);
            digitalWrite(26, LOW);
        } else {
            digitalWrite(24, LOW);
            digitalWrite(25, LOW);
            digitalWrite(26, HIGH);
        }

        if (pidFactor >= 0) {
            //move the motor one way
            /* TODO: Correct pins
            digitalWrite(A8, 1);
            digitalWrite(A9, 0);
            analogWrite(A7, analogOut);
            */
        } else {
            //move the motor the other way
            /* TODO: Correct pins
            digitalWrite(A8, 0);
            digitalWrite(A9, 1);
            analogWrite(A7, -analogOut);
            */
        }
        previousError = currentError;

    }

    /**
     * Sets the desired value for the x rotation
     * @param x The desired value for x rotation
     */
    void PID::setDesiredX(int x) {
        desiredX = x;
    }

    /**
     * getter for the desired x
     * @return desired x
     */
    int PID::getDesiredX() const {
        return desiredX;
    }

    void PID::setPID(double p, double i, double d) {
        P = p;
        I = i;
        D = d;
    }

    double PID::getP() {
        return P;
    }

    void PID::idleMotor() {
        /* TODO: Correct pins!
        digitalWrite(A8, 0);
        digitalWrite(A9, 1);
        analogWrite(A7, 0);
        digitalWrite(24, LOW);
        digitalWrite(25, LOW);
        digitalWrite(26, LOW);
        */
    }
}
