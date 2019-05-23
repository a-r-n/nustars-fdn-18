#include <Servo.h>

#define ADSpin 5

Servo ADSservo;
volatile int servoPos = 0;
int interruptpin = 13;
int interruptpin2 = 10;

void setup() {
  pinMode(interruptpin, INPUT_PULLUP);
  pinMode(interruptpin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptpin), ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptpin2), ISR2, FALLING);
  ADSservo.attach(5);
  Serial.println("Setup");
  delay(200);
  ADSservo.writeMicroseconds(1500);
}

void ISR(){
  servoPos = 1300;
}
void ISR2(){
  servoPos = 1500;
}

void loop() {
  ADSservo.writeMicroseconds(servoPos);
  Serial.println(servoPos);
  delay(20);

}
