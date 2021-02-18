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

#ifndef SENSORSHELMET_H_
#define SENSORSHELMET_H_

#include "BMI088.h"
#include <Adafruit_H3LIS331.h>
#include "communication.h"


/* Accelerometer for large impacts */
extern Adafruit_H3LIS331 lis;

/* Accelerometers of IMUs */
extern Bmi088Accel accel_1;
extern Bmi088Accel accel_2;

/* Gyroscopes of IMUs */
extern Bmi088Gyro gyro_1;
extern Bmi088Gyro gyro_2;


/* Function starting all 3 sensors and the required I2C buses*/
void setup_sensors();


/* Function reading data from all 3 sensors */
void read_sensors();

/* Function returning acceleration along the x-axis according to the big accelerometer */
int16_t getBigAccX();

/* Function returning acceleration along the y-axis according to the big accelerometer */
int16_t getBigAccY();

/* Function returning acceleration along the z-axis according to the big accelerometer */
int16_t getBigAccZ();


/* Function printing linear acceleration from IMUs in m/s^2 */
void print_accel(Bmi088Accel accel, int accelNb);

/* Function printing angular acceleration from IMUs in rad/s */
void print_gyro(Bmi088Gyro gyro, int gyroNb);

/* Function printing linear acceleration from big accelerometer in m/s^2 */
void print_big_accel();


/* Function copying sensor readings in message structure*/
struct_message copy_readings(struct_message readings);

#endif // SENSORSHELMET_H_
