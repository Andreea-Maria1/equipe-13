#include "DriveBase.h"
#include "CapteurUltrasonique.h"  
#include "customPID.h"

#define FR_MOTOR_PIN 2
#define FL_MOTOR_PIN 1
#define BR_MOTOR_PIN 3
#define BL_MOTOR_PIN 4

AF_DCMotor FR_Motor(FR_MOTOR_PIN);
AF_DCMotor FL_Motor(FL_MOTOR_PIN);
AF_DCMotor BR_Motor(BR_MOTOR_PIN);
AF_DCMotor BL_Motor(BL_MOTOR_PIN);

DriveBase robot(&FR_Motor, &FL_Motor, &BR_Motor, &BL_Motor);
customPID pid;

void setup() {
  Serial.begin(250000);
  
  while (!Serial) {
    ;
  }
}

void loop() {
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(receivedData);
    receivedData.trim();

    if (receivedData == "forward") {
      robot.drive(200, 0, 0);
    }
    else if (receivedData == "backward") {
      robot.drive(-200, 0, 0);
    }
    else if (receivedData == "left") {
      robot.drive(0, -200, 0);
    }
    else if (receivedData == "right") {
      robot.drive(0, 200, 0);
    }
    else if (receivedData == "rotate_left") {
      robot.drive(0, 0, -200);
    }
    else if (receivedData == "rotate_right") {
      robot.drive(0, 0, 200);
    }
    else if (receivedData == "stop") {
      robot.drive(0, 0, 0);
    }
    else {
      Serial.println("Unknown receivedData.");
    }
    delay(10);
  }
}
