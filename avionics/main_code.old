#include "sensors.h"
#include "storage.h"
#include "pid.h"
#include "radio.h"
#include <string>

//#include "Arduino"

using namespace nustars;

//Constants
const long BROADCAST_DELAY = 0;

//Evil globals
bool automaticLaunchDetected = false;
bool zeroAccelerationDetected = false;
bool usingGroundAutoLaunch = true;
long timeDetectedStartCondition = LONG_MAX;


//initialize the various classes
Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
GPS* gps = NULL;
PID* pid= NULL;
Radio* radio = NULL;
Storage* storage = NULL;

long lastLoopTime = 0;
long lastBroadcast = 0;

const int LED_PINS[] = {24, 25, 26}; //where are the leds
const char FILE_NAME[] = "out.txt"; //file to write logs to

//user-defined tracking info (defaults, can be overwritten by ground station)
int trackingTargets[] = {180, 90, 0, 0, 0, 0}; //tracking programming, hardcoded to accept 6 values
int trackingDelays[] = {10000, 1000, 1000, 0, 0, 0}; //tracking delays, hardcoded to accept 6 values

bool forceUseGroundTrackingState = false;
bool groundTrackingState = false;

//PID information
unsigned int P = 1000;
unsigned int I = 0;
unsigned int D = 0;
double defaultPIDs[] = {0, 0, 0};

//TODO: Remember what this does
unsigned long transmission = 0;

//internal tracking variables
unsigned long timeStartedThisTrack = 0;
int currentTrack = 0;
bool everTracked = false;
bool trackingComplete = false;

void setup() {
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);

    //PIN SETUP
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    pinMode(A9, OUTPUT); //motor direction input A
    pinMode(A8, OUTPUT); //motor direction input B
    pinMode(A7, 0); //controls motor speed

    Serial.begin(9600); //USB
    delay(1000);
    Serial1.begin(9600); //GPS
    Serial2.begin(9600); //RADIO
    //Initialize classes
    accelerometer = new Accelerometer;
    altimeter = new Altimeter;
    gps = new GPS;
    pid = new PID;
    radio = new Radio;
    storage = new Storage("out.txt");
    //Serial.println("Startup complete!");
}

void loop() {
    long thisTime = millis();
    altimeter->tick();
    accelerometer->tick();
    gps->tick();

    //grab the orientations
    int x_rot, y_rot, z_rot;
    x_rot = accelerometer->getOrientation(X_AXIS);
    y_rot = accelerometer->getOrientation(Y_AXIS);
    z_rot = accelerometer->getOrientation(Z_AXIS);

    //determine whether or not we are tracking and decide tracking target

	bool trackingDetected = (forceUseGroundTrackingState && groundTrackingState) || ((!forceUseGroundTrackingState && automaticLaunchDetected && usingGroundAutoLaunch) || (!forceUseGroundTrackingState && zeroAccelerationDetected && !usingGroundAutoLaunch);

	//register detection of launch conditions
	//separate lines because I can
	if (!everTracked && trackingDetected) {
		timeDetectedStartCondition = thisTime;
	}

	if (trackingDetected && (thisTime - timeDetectedStartCondition > 3000) && !trackingComplete) {
        Serial.printf("%d\n", trackingDelays[currentTrack]);
        pid->setDesiredX(trackingTargets[currentTrack]);
        pid->tick(x_rot);
        if (!everTracked) {
            everTracked = true;
            timeStartedThisTrack = thisTime;
        }
        if (thisTime - timeStartedThisTrack > trackingDelays[currentTrack]) {
            Serial.printf("%s%d\n", "tracked time was: ", (int)(thisTime - timeStartedThisTrack));
            timeStartedThisTrack = thisTime;
            currentTrack++;
            if (currentTrack >= 6) trackingComplete = true;
        }
    } else {
        pid->idleMotor();
    }


    //do everything involving radio broadcast
    if (thisTime - lastBroadcast > BROADCAST_DELAY) { //so we don't kill the radios
        char* msg = new char[100];
        sprintf(msg, "NU T:%lu/X:%d/Y:%d/Z:%d/Tr:%d/Ln:%.4f/Lt:%.4f/Lp:%lu/A:%d/Tn:%lu/\n",
                thisTime, x_rot, y_rot, z_rot, pid->getDesiredX(), gps->getLng(),
                gps->getLat(), (thisTime - lastLoopTime), altimeter->getAltitude(), ++transmission);
        Serial2.printf(msg);
        storage->write(msg);
        delete msg;
    }

    //do everything involving the radio reciever
    const int WAIT_PACKET_CNT = 50;
    if (Serial2.available() >= WAIT_PACKET_CNT) {
        char* s = new char[WAIT_PACKET_CNT]{0};
        for (int i = 0; i < WAIT_PACKET_CNT; i++) {
            s[i] = Serial2.read();
        }
        Serial2.flush();
        bool pb = false, ib = false, db = false;
        bool usingGSPID = false;
        for (int j = 0; j < WAIT_PACKET_CNT - 2; j++) {
            if (s[j] == 'N' && s[j+1] == 'U') {
                for (int i = 0; i < WAIT_PACKET_CNT - 2; i++) {
                    if (s[i] == 'p' && s[i + 1] == '/') {
                        usingGSPID = s[i + 2];
                    } else if (s[i] == 'f' && s[i + 1] == '/') {
                        forceUseGroundTrackingState = s[i + 2];
                    } else if (s[i] == 's' && s[i + 1] == '/') {
                        groundTrackingState = s[i + 2];
                    } else if (s[i] == 'P' && s[i + 1] == '/') {
                        P = 0;
                        for (int n = 0; n < 2; n++) {
                            P = (P << 8) + s[i + 2 + n];
                            pb = true;
                        }
                    } else if (s[i] == 'I' && s[i + 1] == '/') {
                        I = 0;
                        for (int n = 0; n < 2; n++) {
                            I = (I << 8) + s[i + 2 + n];
                            ib = true;
                        }
                    } else if (s[i] == 'D' && s[i + 1] == '/') {
                        D = 0;
                        for (int n = 0; n < 2; n++) {
                            D = (D << 8) + s[i + 2 + n];
                            db = true;
                        }
                    }
                    if (pb && ib && db) {
                        break;
                    }
                }
                break;
            } else if (s[j] == 'N' && s[j+1] == 'X') {
                for (int i = 0; i < WAIT_PACKET_CNT - 6; i++) {
                    if (s[i] == 'a') {
                        for (int k = 0; k < 6; k++) {
                            trackingTargets[k] = 0;
                            for (int n = 0; n < 2; n++) {
                                trackingTargets[k] =
                                        (trackingTargets[k] << 8) + s[i + 1 + 2*k + n];
                            }
                        }
                    } else if (s[i] == 'i') {
                        for (int k = 0; k < 6; k++) {
                            trackingDelays[k] = 0;
                            for (int n = 0; n < 2; n++) {
                                trackingDelays[k] =
                                        (trackingDelays[k] << 8) + s[i + 1 + 2*k + n];
                                Serial.printf("%s%d\n", "I got: ", trackingDelays[k]);
                            }
                        }
                    }
                }
            }
        }
        if (usingGSPID) pid->setPID(P/1000.0, I/1000.0, D/1000.0);
        else pid->setPID(defaultPIDs[0], defaultPIDs[1], defaultPIDs[2]);
        delete[] s;
    }


    //handle the launch detection
    // yes this is 39 on purpose it does not go higher this is so sad
    if (accelerometer->getAcceleration(Y_AXIS) == 39 || accelerometer->getAcceleration(Y_AXIS) == -39) {
        automaticLaunchDetected = true;
    }
    //detect end of acceleration
    if (automaticLaunchDetected && accelerometer->getAcceleration(Y_AXIS) < 9 && accelerometer->getAcceleration(Y_AXIS) > -9) {
        zeroAccelerationDetected = true;
    }
    Serial.println(thisTime - lastLoopTime);
    lastLoopTime = thisTime;
}

