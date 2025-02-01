#include "Arduino.h"
#include <AFMotor.h>
#include "DriveBase.h"

DriveBase::DriveBase(AF_DCMotor* FR_Motor, AF_DCMotor* FL_Motor, AF_DCMotor* BR_Motor, AF_DCMotor* BL_Motor) {
  this->FR_Motor = FR_Motor;
  this->FL_Motor = FL_Motor;
  this->BR_Motor = BR_Motor;
  this->BL_Motor = BL_Motor;
}

void DriveBase::drive(int vx, int vy, int angularVelocity) {
  int FR_speed = vy + vx + angularVelocity;
  int FL_speed = vy - vx - angularVelocity;
  int BR_speed = vy - vx + angularVelocity;
  int BL_speed = vy + vx - angularVelocity;

  // Si vous souhaitez utiliser dataToSend, déclarez-le correctement.
  // Sinon, vous pouvez laisser ces lignes commentées ou les supprimer.
  // dataToSend->FR_Motor_Speed_Data = FR_speed;
  // dataToSend->FL_Motor_Speed_Data = FL_speed;
  // dataToSend->BR_Motor_Speed_Data = BR_speed;
  // dataToSend->BL_Motor_Speed_Data = BL_speed;

  setMotor(FR_Motor, FR_speed);
  setMotor(FL_Motor, FL_speed);
  setMotor(BR_Motor, BR_speed);
  setMotor(BL_Motor, BL_speed);
}

void DriveBase::setMotor(AF_DCMotor* motor, int speed) {
  if (speed == 0) {
    motor->run(RELEASE);
  } else {
    motor->run(speed < 0 ? BACKWARD : FORWARD);
  }
  
  speed = constrain(abs(speed), 0, 240);
  motor->setSpeed(speed);
}
