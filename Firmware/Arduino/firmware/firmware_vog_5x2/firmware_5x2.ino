// Version 1.0.0

#include <Servo.h>

Servo myservo;


const int servoPin = 4;
const int positionMove = 700;
const int positionStop = 1500;
const int minOpenMagnet = 514; 
const int maxOpenMagnet = 525;
const int minClosedMagnet = 514; 
const int maxClosedMagnet = 525;

const int hallSensorOpenPin = 3;
const int hallSensorClosedPin = 2;

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
    Serial.println(readHallSensor(hallSensorClosedPin));
    Serial.println(readHallSensor(hallSensorOpenPin));
}

void loop() {
    int closedStateValue = readHallSensor(hallSensorClosedPin);
    int openStateValue = readHallSensor(hallSensorOpenPin);

    Serial.println("Closed");
    Serial.println(readHallSensor(hallSensorClosedPin));
    Serial.println("Open");
    Serial.println(readHallSensor(hallSensorOpenPin));

    if (closedStateValue < minClosedMagnet || closedStateValue > maxClosedMagnet) {
        closedStateDetected = true;
    }

    if (openStateValue < minOpenMagnet || openStateValue > maxOpenMagnet) {
        openStateDetected = true;
    }

    delay(20); // Delay to prevent excessive servo movement or signal processing

    if (refilled != 0) {
        if (processSignal() && refilled > 0) {
            Serial.println("Refill Started");
            myservo.writeMicroseconds(positionMove);
        }

        if (refilled != 0 && closedStateDetected && (openStateValue < minOpenMagnet || openStateValue > maxOpenMagnet)) {
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
        if (openStateDetected && (closedStateValue < minClosedMagnet || closedStateValue > maxClosedMagnet)) {
            Serial.println("Drop activated");
            Serial.println(closedStateValue);
            Serial.println(openStateValue);
            myservo.writeMicroseconds(positionStop);
            openStateDetected = false;
        }
    }
}
