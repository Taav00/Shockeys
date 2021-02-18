/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Definition: Implements all functions, variables, structures and constants needed
to communicate in between the helmet and the routeur.
*/

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 			1
#define PRINTSCANRESULTS 	0

// Global copy of slave
extern esp_now_peer_info_t slave;

//Structure to send data
//Must match the receiver's structure
struct struct_message {

    float IMU1_accX=0;
    float IMU1_accY=0;
    float IMU1_accZ=0;
    float IMU1_gyrX=0;
    float IMU1_gyrY=0;
    float IMU1_gyrZ=0;

    float IMU2_accX=0;
    float IMU2_accY=0;
    float IMU2_accZ=0;
    float IMU2_gyrX=0;
    float IMU2_gyrY=0;
    float IMU2_gyrZ=0;

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

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

// Return string indicating if sending data was successful
String checkSuccess();

// Scan for slaves in AP mode
void ScanForSlave();

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave();

// Function setting up ESP-NOW communication
void setupESPNOW();

#endif // COMMUNICATION_H_
