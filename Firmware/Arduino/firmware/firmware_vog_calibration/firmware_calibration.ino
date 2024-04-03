#include <Servo.h>

Servo myservo;

const int servoPin = 4;
const int positionMove = 700; 
const int positionStop = 1500;

const int hallSensorOpenPin = 2;
const int hallSensorClosedPin = 3;

const int activationPin = A1;

unsigned long startTime;
const long calibrationPeriod = 10000;
bool calibrationCompleted = false;
int minMagnetValue = 1023;
int maxMagnetValue = 0;


int readHallSensor(int pin) {
    return analogRead(pin);
}

void setup() {
    myservo.attach(servoPin);
    pinMode(hallSensorOpenPin, INPUT);
    pinMode(hallSensorClosedPin, INPUT);
    Serial.begin(9600);
    Serial.println("---------- Multi Drop Calibration Version 1.0.0 ------------");
    startTime = millis(); 
}

void loop() {
    if (!calibrationCompleted) {
        if (millis() - startTime < calibrationPeriod) {
           
            myservo.writeMicroseconds(positionMove);
            
            int sensorValue = readHallSensor(hallSensorClosedPin);
            minMagnetValue = min(minMagnetValue, sensorValue);
            maxMagnetValue = max(maxMagnetValue, sensorValue);
        } else {
            myservo.writeMicroseconds(positionStop);
            calibrationCompleted = true;
            Serial.print("Calibration Completed. Min: ");
            Serial.print(minMagnetValue);
            Serial.print(", Max: ");
            Serial.println(maxMagnetValue);
        }
    }

}
