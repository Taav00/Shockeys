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

#include "communication_helmet.h"
#include "communication_app.h"


// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Creat an array to hold incoming sensor readings
String total_string = "";

// Config AP SSID
void configDeviceAP() {
  const char *SSID = "Router";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}


// Print incoming readings from IMU1 in Serial Monitor
void updateIMUDisplay(){
  
  Serial.println("INCOMING READINGS");
  Serial.print("Acceleration in X: ");
  Serial.print(incomingReadings.IMU1_accX);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Y: ");
  Serial.print(incomingReadings.IMU1_accY);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Z: ");
  Serial.print(incomingReadings.IMU1_accZ);
  Serial.println(" m/s^2");

  Serial.print("Gyroscope in X: ");
  Serial.print(incomingReadings.IMU1_gyrX);
  Serial.println(" rad/s^2");
  Serial.print("Gyroscope in Y: ");
  Serial.print(incomingReadings.IMU1_gyrY);
  Serial.println(" rad/s^2");
  Serial.print("Gyroscope in Z: ");
  Serial.print(incomingReadings.IMU1_gyrZ);
  Serial.println(" rad/s^2");
  Serial.println();
}


// Print incoming readings from accelerometer in Serial Monitor
void updateAccDisplay(){
  Serial.println("INCOMING READINGS");
  Serial.print("Acceleration in X: ");
  Serial.print(incomingReadings.LIS_accX);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Y: ");
  Serial.print(incomingReadings.LIS_accY);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Z: ");
  Serial.print(incomingReadings.LIS_accZ);
  Serial.println(" m/s^2");
  Serial.println();
}


// Callback when data is received from helmet
void OnHelmetDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //Serial.print("Bytes received: ");
  //Serial.println(len);

  // Print the data sender MAC address
  Serial.print("Helmet recv_cb, from mac: ");
  char macString[50] = {0};
  sprintf(macString, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(macString);
  
  // Print data as soon as you receive it 
  updateIMUDisplay();

  // Concatenate incoming readings into a string
  total_string = "";
  total_string += incomingReadings.IMU1_accX;
  total_string += ",";
  total_string += incomingReadings.IMU1_accY;
  total_string += ",";
  total_string += incomingReadings.IMU1_accZ;
  total_string += ",";
  total_string += incomingReadings.IMU2_accX;
  total_string += ",";
  total_string += incomingReadings.IMU2_accY;
  total_string += ",";
  total_string += incomingReadings.IMU2_accZ;
  total_string += ",";
  total_string += incomingReadings.LIS_accX;
  total_string += ",";
  total_string += incomingReadings.LIS_accY;
  total_string += ",";
  total_string += incomingReadings.LIS_accZ;
  total_string += ",";
  total_string += incomingReadings.IMU1_gyrX;
  total_string += ",";
  total_string += incomingReadings.IMU1_gyrY;
  total_string += ",";
  total_string += incomingReadings.IMU1_gyrZ;
  total_string += ",";
  total_string += incomingReadings.IMU2_gyrX;
  total_string += ",";
  total_string += incomingReadings.IMU2_gyrY;
  total_string += ",";
  total_string += incomingReadings.IMU2_gyrZ;

  // Convert string with incoming readings to
  // a char array for the BLE transfer
  int n = total_string.length();
  char sensors_readings[n + 1]; // +1 for the NULL at the end of the string (by default)
  strcpy(sensors_readings,total_string.c_str());

  Serial.println(sensors_readings);
  
  // Send sensors data to app
  sendDataBLE(sensors_readings);
  
 
}


// Setup device as a slave (receives data via ESP-NOW)
void setupESPNOW(){
  
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnHelmetDataRecv);
}
