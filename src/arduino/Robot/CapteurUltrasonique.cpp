#include "CapteurUltrasonique.h"

CapteurUltrasonique::CapteurUltrasonique(uint8_t trigPin, uint8_t echoPin) :
    trigPin(trigPin),
    echoPin(echoPin),
    previousTime(0),
    triggerSent(false)
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

float CapteurUltrasonique::getDistance() {
    long duration, distance;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);

    distance = duration * 0.034 / 2;

    if (distance > 0 && distance <= 200 && distance != 65535) {
        return distance;
    } else {
        return -1;
    }
}
