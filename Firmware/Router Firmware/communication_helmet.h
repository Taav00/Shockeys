/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Description: Implements all functions, variables, structures and constants needed
to communicate in between the router and the helmet using ESP-NOW.
*/


#ifndef COMMUNICATION_HELMET_H_
#define COMMUNICATION_HELMET_H_

#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL           1

// Structure to receive data from helmet
// Must match the sender's structure
struct struct_message {

    float IMU1_accX;
    float IMU1_accY;
    float IMU1_accZ;
    float IMU1_gyrX;
    float IMU1_gyrY;
    float IMU1_gyrZ;

    float IMU2_accX;
    float IMU2_accY;
    float IMU2_accZ;
    float IMU2_gyrX;
    float IMU2_gyrY;
    float IMU2_gyrZ;

    int16_t LIS_accX;
    int16_t LIS_accY;
    int16_t LIS_accZ;

    // This constructor does no initialization.
    struct_message() { }

    // This constructor initializes all variables to the same value
    struct_message(int16_t i) : IMU1_accX(i), IMU1_accY(i), IMU1_accZ(i), 
                                IMU1_gyrX(i), IMU1_gyrY(i), IMU1_gyrZ(i), 
                                IMU2_accX(i), IMU2_accY(i), IMU2_accZ(i), 
                                IMU2_gyrX(i), IMU2_gyrY(i), IMU2_gyrZ(i),
                                LIS_accX(i), LIS_accY(i), LIS_accZ(i) { }

};

// Config AP SSID
void configDeviceAP();

// Print incoming readings from IMU1 in Serial Monitor
void updateIMUDisplay();

// Print incoming readings from accelerometer in Serial Monitor
void updateAccDisplay();

// Callback when data is received from helmet
void OnHelmetDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

// Setup device as a slave (receives data via ESP-NOW)
void setupESPNOW();



#endif // COMMUNICATION_HELMET_H_
