/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Definition: Implements all functions, variables and constants needed by
sensors.
*/

#include "sensorsHelmet.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>


// Bus addresses for sensors
#define ADD_ACC_1       0x18
#define ADD_ACC_2       0x19
#define ADD_ACC_3       0x19 // same as ADD_ACC_2 but on a different I2C bus
#define ADD_GYRO_1      0x68
#define ADD_GYRO_2      0x69

// 1st I2C bus -> for IMUs
#define I2C_1_SDA       25
#define I2C_1_SCL       26

// 2nd I2C bus -> for big accelerometer
#define I2C_2_SDA       16
#define I2C_2_SCL       17

#define I2C_FREQ        400000

#define H3LIS_RANGE     H3LIS331_RANGE_100_G       
#define LIS_DATARATE    LIS331_DATARATE_1000_HZ

TwoWire I2Cone = TwoWire(1);
TwoWire I2Ctwo = TwoWire(2);


/* Accelerometers of IMUs */ 
Bmi088Accel accel_1(I2Cone,ADD_ACC_1);
Bmi088Accel accel_2(I2Cone,ADD_ACC_2);

/* Gyroscopes of IMUs */
Bmi088Gyro gyro_1(I2Cone,ADD_GYRO_1);
Bmi088Gyro gyro_2(I2Cone,ADD_GYRO_2);

/* Accelerometer for large impacts */ 
Adafruit_H3LIS331 lis = Adafruit_H3LIS331();

/* Structure to store the most recent event from sensor LIS*/
sensors_event_t event;


/* Function starting all 3 sensors and the required I2C buses*/
void setup_sensors(){

  int status;
  
  // setup I2C buses
  I2Cone.begin(I2C_1_SDA,   I2C_1_SCL,  I2C_FREQ);
  delay(50);
  I2Ctwo.begin(I2C_2_SDA,   I2C_2_SCL,  I2C_FREQ);
  delay(50);

  // start big accelerometer
  if (! lis.begin_I2C(ADD_ACC_3, &I2Ctwo)) {   
    Serial.println("Couldnt start H3LIS");
    while (1) yield();
  }
  delay(50);
  lis.setRange(H3LIS_RANGE);   
  lis.setDataRate(LIS_DATARATE);

  while(!Serial) {}
  
  // start IMU sensors
  status = accel_1.begin();
  if (status < 0) {
  // if (false) {
    Serial.println("Accel Initialization Error : Accel 1");
    Serial.println(status);
    while (1) {}
  }
  status = accel_2.begin();
  if (status < 0) {
    Serial.println("Accel Initialization Error : Accel 2");
    Serial.println(status);
    while (1) {}
  }
  
  status = gyro_1.begin();
  if (status < 0) {
  // if (false) {
    Serial.println("Gyro Initialization Error : Gyro 1");
    Serial.println(status);
    while (1) {}
  }
  status = gyro_2.begin();
  if (status < 0) {
    Serial.println("Gyro Initialization Error : Gyro 2");
    Serial.println(status);
    while (1) {}
  }
}


/* Function reading data from all 3 sensors */
void read_sensors(){
  
  // read the IMUs linear accelerations 
  accel_1.readSensor();
  accel_2.readSensor();
  
  // read the IMUs angular accelerations
  gyro_1.readSensor();
  gyro_2.readSensor();
  
  // read linear accelerations from 200g accelerometer
  lis.read();      // get X Y and Z data at once
  // or....get a new sensor event, normalized
  lis.getEvent(&event);

}

/* Function returning acceleration along the x-axis according to the big accelerometer */
int16_t getBigAccX(){
  return event.acceleration.x;
}

/* Function returning acceleration along the y-axis according to the big accelerometer */
int16_t getBigAccY(){
  return event.acceleration.y;
}

/* Function returning acceleration along the z-axis according to the big accelerometer */
int16_t getBigAccZ(){
  return event.acceleration.z;
}


/* Function printing linear acceleration from IMUs in m/s^2 */
void print_accel(Bmi088Accel accel, int accelNb){
  Serial.printf("Accel_X_%d: %f /t", accelNb, accel.getAccelX_mss());
  Serial.printf("Accel_Y_%d: %f /t", accelNb, accel.getAccelY_mss());
  Serial.printf("Accel_Z_%d: %f /n", accelNb, accel.getAccelZ_mss());
}

/* Function printing angular acceleration from IMUs in rad/s */
void print_gyro(Bmi088Gyro gyro, int gyroNb){
  Serial.printf("Gyro_X_%d: %f /t", gyroNb, gyro.getGyroX_rads());
  Serial.printf("Gyro_Y_%d: %f /t", gyroNb, gyro.getGyroY_rads());
  Serial.printf("Gyro_Z_%d: %f /n", gyroNb, gyro.getGyroZ_rads());
}

/* Function printing linear acceleration from big accelerometer */
void print_big_accel(){
  // Display results as m/s^2 in a plot-friendly format
  Serial.printf("Accel_X_3 : %f /t", event.acceleration.x);
  Serial.printf("Accel_Y_3 : %f /t", event.acceleration.y);
  Serial.printf("Accel_Z_3 : %f /n", event.acceleration.z);
}

/* Function copying sensor readings in message structure*/
struct_message copy_readings(struct_message readings){
   
   readings.IMU1_accX = accel_1.getAccelX_mss();
   readings.IMU1_accY = accel_1.getAccelY_mss();
   readings.IMU1_accZ = accel_1.getAccelZ_mss();
   readings.IMU1_gyrX = gyro_1.getGyroX_rads();
   readings.IMU1_gyrY = gyro_1.getGyroY_rads();
   readings.IMU1_gyrZ = gyro_1.getGyroZ_rads();

   readings.IMU2_accX = accel_2.getAccelX_mss();
   readings.IMU2_accY = accel_2.getAccelY_mss();
   readings.IMU2_accZ = accel_2.getAccelZ_mss();
   readings.IMU2_gyrX = gyro_2.getGyroX_rads();
   readings.IMU2_gyrY = gyro_2.getGyroY_rads();
   readings.IMU2_gyrZ = gyro_2.getGyroZ_rads();

   readings.LIS_accX = event.acceleration.x;
   readings.LIS_accY = event.acceleration.y;
   readings.LIS_accZ = event.acceleration.z;

   return readings;
}
