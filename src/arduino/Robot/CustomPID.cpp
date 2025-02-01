#include "customPID.h"

customPID::customPID():
  P_Z(0),
  I_Z(0),
  D_Z(0),
  P_X(0),
  D_X(0),
  P_Y(0),
  D_Y(0),
  previousErrorZ(0),
  previousMillisZ(0)
{

}

float customPID::shortestAngularDistance(float from, float to) {
    float delta = to - from;
    
    if (delta < -180) {
        delta += 360;
    } 
    else if (delta >= 180) {
        delta -= 360;
    }

    return delta;
}

float customPID::xCorrection(float setPoint, float distance){
  float error = setPoint - distance;

  P_X = KP_X * error;

  float dx = error - previousErrorX;
  previousErrorX = error;

  D_X = KD_X * error;

  return constrain(P_X + D_X, -240, 240);
}


float customPID::zCorrection(float setPoint, float angle){

  unsigned long elaspedTime = (millis() - previousMillisZ);  

  float error = shortestAngularDistance(angle, setPoint);  

  if(elaspedTime > sampleRateZ){ 
    previousMillisZ = millis();  
    previousErrorZ = error;   
    I_Z += (KI_Z * error);     
  }

  float dx = error - previousErrorZ; 

  P_Z = KP_Z * error;               
  D_Z = KD_Z * dx;                

  return P_Z + D_Z;                   
}