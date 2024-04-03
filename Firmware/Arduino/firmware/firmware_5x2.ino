// Version 1.0.0

#include <Servo.h>

Servo myservo;

const int servoPin = 4;
const int positionMove = 700;
const int positionStop = 1500;
const int minMagnet = 514;
const int maxMagnet = 525;

const int hallSensorOpenPin = 2;
const int hallSensorClosedPin = 3;

const int activationPin = A1;

bool lastSignalState = false;
bool closedStateDetected = false;
bool openStateDetected = false;
int refilled = 5;
bool refillCompleted = false;

bool dropActive = false;

bool processSignal() {
    static int thresholdOn = 1900;
    static int thresholdOff = 1000;
    int duration = pulseIn(activationPin, HIGH);

    if (duration > thresholdOn && !lastSignalState) {
        lastSignalState = true;
        Serial.println("Rotate");
        return true;
    } else if (duration < thresholdOff && lastSignalState) {
        lastSignalState = false;
    }
    return false;
}

int readHallSensor(int pin) {
    return analogRead(pin);
}

void setup() {
    myservo.attach(servoPin);
    pinMode(hallSensorOpenPin, INPUT);
    pinMode(hallSensorClosedPin, INPUT);
    Serial.begin(9600);
    Serial.println("---------- Multi Drop Version 1.0.0 ------------");
}

void loop() {
    int closedStateValue = readHallSensor(hallSensorClosedPin);
    int openStateValue = readHallSensor(hallSensorOpenPin);

    if (closedStateValue < minMagnet || closedStateValue > maxMagnet) {
        closedStateDetected = true;
    }

    if (openStateValue < minMagnet || openStateValue > maxMagnet) {
        openStateDetected = true;
    }

    delay(20); // Delay to prevent excessive servo movement or signal processing

    if (refilled != 0) {
        if (processSignal() && refilled > 0) {
            Serial.println("Refill Started");
            myservo.writeMicroseconds(positionMove);
        }

        if (refilled != 0 && closedStateDetected && (openStateValue < minMagnet || openStateValue > maxMagnet)) {
            Serial.println("Refill Waiting");
            Serial.println(closedStateValue);
            Serial.println(openStateValue);
            myservo.writeMicroseconds(positionStop);
            refilled--;
            closedStateDetected = false;
        }

        if (refilled == 0) {
            Serial.println("Refill Completed");
            Serial.println(refilled);
            refillCompleted = true;
        }
    }

    if (refillCompleted) {
        if (processSignal()) {
            Serial.println("Drop started");
            myservo.writeMicroseconds(positionMove);
            dropActive = true;
        }
        if (openStateDetected && (closedStateValue < minMagnet || closedStateValue > maxMagnet)) {
            Serial.println("Drop activated");
            Serial.println(closedStateValue);
            Serial.println(openStateValue);
            myservo.writeMicroseconds(positionStop);
            openStateDetected = false;
        }
    }
}
