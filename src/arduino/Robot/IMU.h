#ifndef IMU_h
#define IMU_h
#include "I2Cdev.h"
#include "Arduino.h"
#include "Wire.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define INTERRUPT_PIN 19

class IMU {
  public :
    IMU(MPU6050* mpu);            // Constructeur de la clase
    void initIMU();               // Initialise l'IMU
    void readAngle();             // Lit les angles
    static void dmpDataWrapper(); // Méthode static pour envelopper les données DMP
    float getX();                 // Retourne l'angle en X
    float getY();                 // Retourne l'angle en Y
    float getZ();                 // Retourne l'angle en Z
    float getAccelX();
    float getAccelY();
    float getAccelZ();

  private :
    MPU6050* mpu;           // Pointeur vers l'object MPU6050

    bool dmpReady;          // Indique si DMP est prêt
    volatile bool mpuInterrupt; // Flag d'interruption du MPU
    uint8_t mpuIntStatus;   // Le status de l'interrupt du MPU
    uint8_t devStatus;      // 0 = fonctionner, 1 = erreur
    uint16_t packetSize;    // La grandeur du packet size attendus(defaut = 42 bytes)
    uint16_t fifoCount;     // Compte les bytes dans le FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    Quaternion q;           // [w, x, y, z]         quaternion 
    VectorFloat gravity;    // [x, y, z]            vecteur gravité
    VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;

    float ypr[3];
    float x;
    float y;
    float z;      
    float accel_x;
    float accel_y;
    float accel_z;

    void dmpDataReady();  
};

#endif