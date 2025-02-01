// Le code pour la voiture

#include "DriveBase.h"

#define FR_MOTOR_PIN 2
#define FL_MOTOR_PIN 1
#define BR_MOTOR_PIN 3
#define BL_MOTOR_PIN 4

AF_DCMotor FR_Motor(FR_MOTOR_PIN);
AF_DCMotor FL_Motor(FL_MOTOR_PIN);
AF_DCMotor BR_Motor(BR_MOTOR_PIN);
AF_DCMotor BL_Motor(BL_MOTOR_PIN);

DriveBase robot(&FR_Motor, &FL_Motor, &BR_Motor, &BL_Motor);

void setup(){
  Serial.begin(250000);

  robot.drive(200,200,200);
}