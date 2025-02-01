#include "IMU.h"

IMU* imuInstancePtr;

IMU::IMU(MPU6050* mpu) :
  dmpReady(false),   
  mpuInterrupt(false)     
{
  this->mpu = mpu;
}


void IMU::dmpDataReady() {
  mpuInterrupt = true;
}

void IMU::dmpDataWrapper() {
  IMU* imuInstance = imuInstancePtr;
  imuInstance->dmpDataReady();
}

void IMU::initIMU() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); 
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif 

  imuInstancePtr = this;
  mpu->initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  Serial.println(mpu->testConnection() ? F("MPU6050 connection réussite") : F("MPU6050 connection à échoué"));

  devStatus = mpu->dmpInitialize();

  mpu->setXGyroOffset(220);
  mpu->setYGyroOffset(76);
  mpu->setZGyroOffset(-85);
  mpu->setZAccelOffset(1788);

  if(devStatus == 0){
    mpu->CalibrateAccel(6);     
    mpu->CalibrateGyro(6);     
    mpu->PrintActiveOffsets();

    mpu->setDMPEnabled(true);

    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataWrapper, RISING);
    mpuIntStatus = mpu->getIntStatus();

    dmpReady = true; 

    packetSize = mpu->dmpGetFIFOPacketSize();   
  } else {
    Serial.print(F("DMP intialisation à échoué(code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void IMU::readAngle(){
  if(!dmpReady) return;

  if(mpu->dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu->dmpGetQuaternion(&q, fifoBuffer);
    mpu->dmpGetAccel(&aa, fifoBuffer);
    mpu->dmpGetGravity(&gravity, &q);
    mpu->dmpGetYawPitchRoll(ypr, &q, &gravity);
    mpu->dmpGetLinearAccel(&aaReal, &aa, &gravity);

    z = ypr[0] * 180/M_PI;  
    y = ypr[1] * 180/M_PI;  
    x = ypr[2] * 180/M_PI;  

    accel_x = aaReal.x;
    accel_y = aaReal.y;
    accel_z = aaReal.z;
  }
}

float IMU::getX(){
  return x;
}

float IMU::getY(){
  return y;
}

float IMU::getZ(){
  return z;
}

float IMU::getAccelX(){
  return accel_x;
}

float IMU::getAccelY(){
  return accel_y;
}

float IMU::getAccelZ(){
  return accel_z;
}