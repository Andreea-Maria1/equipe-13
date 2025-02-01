#include "DriveBase.h"
#include "CapteurUltrasonique.h"  
#include "customPID.h"
#include "I2Cdev.h"
#include "Arduino.h"
#include "Wire.h"
#include "IMU.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define FR_MOTOR_PIN 2
#define FL_MOTOR_PIN 1
#define BR_MOTOR_PIN 3
#define BL_MOTOR_PIN 4

#define FRONT_TRIG_PIN 34
#define FRONT_ECHO_PIN 32
#define RIGHT_TRIG_PIN 36
#define RIGHT_ECHO_PIN 38
#define LEFT_TRIG_PIN 42 
#define LEFT_ECHO_PIN 40 

#define MIN_DISTANCE 35 

AF_DCMotor FR_Motor(FR_MOTOR_PIN);
AF_DCMotor FL_Motor(FL_MOTOR_PIN);
AF_DCMotor BR_Motor(BR_MOTOR_PIN);
AF_DCMotor BL_Motor(BL_MOTOR_PIN);


CapteurUltrasonique frontDistance(FRONT_TRIG_PIN, FRONT_ECHO_PIN);  
CapteurUltrasonique rightDistance(RIGHT_TRIG_PIN, RIGHT_ECHO_PIN);  
CapteurUltrasonique leftDistance(LEFT_TRIG_PIN, LEFT_ECHO_PIN);
DriveBase robot(&FR_Motor, &FL_Motor, &BR_Motor, &BL_Motor);
customPID pid;
MPU6050 mpu;  
IMU imu(&mpu);   

int autoLoopCounter = 0;
int speedX = 0, speedY = 0, speedZ = 0;  

uint16_t frontDistanceValue = 0;
uint16_t leftDistanceValue = 0;
uint16_t rightDistanceValue = 0;
uint8_t sensorTurn = 0;

void setup() {
  Serial.begin(250000);
  imu.initIMU();  
  
  while (!Serial) {
    ;
  }
}

void loop() {
  imu.readAngle();

  float x = imu.getX();
  float y = imu.getY();
  float z = imu.getZ();

  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(receivedData);
    receivedData.trim();

    if (receivedData == "forward") {
      speedX = 0;
      speedY = 200;
      speedZ = 0;
    }
    else if (receivedData == "backward") {
      speedX = 0;
      speedY = -200;
      speedZ = 0;
    }
    else if (receivedData == "left") {
      speedX = 200;
      speedY = 0;
      speedZ = 0;
    }
    else if (receivedData == "right") {
      speedX = -200;
      speedY = 0;
      speedZ = 0;
    }
    else if (receivedData == "forward-left") {
      speedX = 0;
      speedY = 0;
      speedZ = 200;
    }
    else if (receivedData == "forward-right") {
      speedX = 0;
      speedY = 0;
      speedZ = -200;
    }
    else if (receivedData == "stop") {
      speedX = 0;
      speedY = 0;
      speedZ = 0;
    }
    autoLoopCounter = 0;
    } else {
      Serial.println("Unknown receivedData.");
      autoLoopCounter++;

      if (autoLoopCounter > 10) {
        speedX = 0;
        speedY = 0;
        speedZ = 0;
      }
    }

    delay(10);
    int zAdjustement = (int)(pid.zCorrection(speedZ, z));
    readDistance(); 
    avoidObstacle();
    Serial.print("Speed z : ");
    Serial.println(z);
    robot.drive(speedX, speedY, zAdjustement);
}


void readDistance() {
  // Alternance entre les capteurs ultrasoniques pour lire les distances
  // parce que sinon sa ralenti trop le code
  sensorTurn++;
  switch (sensorTurn) {
    case 1:
      frontDistanceValue = frontDistance.getDistance();
      break;
    case 3:
      leftDistanceValue = leftDistance.getDistance();
      break;
    case 5: 
      rightDistanceValue = rightDistance.getDistance();
      break;
  }
  if (sensorTurn >= 6) {
    sensorTurn = 0;
  }
}

void avoidObstacle() {
  if (isObstacle(leftDistanceValue) && isObstacle(rightDistanceValue)) {
  } else {
    if (isObstacle(leftDistanceValue)) {
      speedX = -pid.xCorrection(MIN_DISTANCE, leftDistanceValue); // Ajustement vers la droite
    }
    if (isObstacle(rightDistanceValue)) {
      speedX = pid.xCorrection(MIN_DISTANCE, rightDistanceValue); // Ajustement vers la gauche
    }
  }
  if (isObstacle(frontDistanceValue)) {
    speedY = -pid.xCorrection(MIN_DISTANCE, frontDistanceValue); // Ajustement en arrière
  }
}

bool isObstacle(float distance) {
  return distance <= MIN_DISTANCE && distance != -1;
}
