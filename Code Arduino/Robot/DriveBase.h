#ifndef DriveBase_h
#define DriveBase_h
#include "Arduino.h"
#include <AFMotor.h>
#include "Radio.h"

class DriveBase {
  public:
    DriveBase(AF_DCMotor* FR_Motor, AF_DCMotor* FL_Motor, AF_DCMotor* BR_Motor, AF_DCMotor* BL_Motor);

    // Méthode pour contrôler le mouvement du robot
    // vx : vitesse linéaire en x (avant-arrière)
    // vy : vitesse linéaire en y (gauche-droite)
    // angularVelocity : vitesse angulaire (rotation)
    void drive(int vx, int vy, int angularVelocity);

  private:
    // Méthode interne pour régler la vitesse d'un moteur
    void setMotor(AF_DCMotor* motor, int speed);

    AF_DCMotor* FR_Motor; // Moteur avant droit
    AF_DCMotor* FL_Motor; // Moteur avant gauche
    AF_DCMotor* BR_Motor; // Moteur arrière gauche
    AF_DCMotor* BL_Motor; // Moteur arrière droit
};

#endif