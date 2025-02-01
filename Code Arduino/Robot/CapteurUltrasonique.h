#ifndef CapteurUltrasonique_h
#define CapteurUltrasonique_h

#include "Arduino.h"

class CapteurUltrasonique {
public:
    CapteurUltrasonique(uint8_t trigPin, uint8_t echoPin);

    float getDistance();

    private:
    uint8_t trigPin;

    uint8_t echoPin;

    unsigned long previousTime;

    bool triggerSent;
};

#endif