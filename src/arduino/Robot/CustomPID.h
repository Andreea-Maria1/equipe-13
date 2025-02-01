#ifndef customPID_h
#define customPID_h
#include "Arduino.h"

#define KP_Z 8
#define KI_Z 0.3
#define KD_Z 100

#define KP_X 4
#define KD_X 5

#define KP_Y 8
#define KD_Y 100

#define sampleRateZ 25

class customPID {
  public :
    customPID();

    float zCorrection(float setPoint, float angle);
    float xCorrection(float setPoint, float distance);

  private : 
    float P_Z;                    
    float I_Z;                    
    float D_Z;                     

    float P_X;
    float D_X;

    float P_Y;
    float D_Y;

    float previousErrorZ;
    float previousErrorX;
    float previousErrorY;
    unsigned long previousMillisZ;  

    float shortestAngularDistance(float from, float to);
};

#endif