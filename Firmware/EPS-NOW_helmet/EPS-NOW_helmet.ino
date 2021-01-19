/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  _
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

#include <Wire.h>
#include "MPU9250.h" // Bolder Flight Systems MPU9250 Library

#define SDA_PIN 25
#define SCL_PIN 26

MPU9250 IMU(Wire,0x68);

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x30, 0xAE, 0xA4, 0x42, 0x89, 0xC4};

// Define variables to store MPU9250 readings to be sent
float accelX;
float accelY;
float accelZ;
float gyroX;
float gyroY;
float gyroZ;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float accX;
    float accY;
    float accZ;
    float gyrX;
    float gyrY;
    float gyrZ;
} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message IMUReadings;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init MPU9250 sensor
  Wire.begin(SDA_PIN, SCL_PIN); 
  bool status = IMU.begin();  
  if (!status) {
    Serial.println("Could not find a valid MPU9250 sensor, check wiring!");
    while (1);
  }

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  getReadings();
 
  // Set values to send
  IMUReadings.accX = accelX;
  IMUReadings.accY = accelY;
  IMUReadings.accZ = accelZ;
  IMUReadings.gyrX = gyroX;
  IMUReadings.gyrY = gyroY;
  IMUReadings.gyrZ = gyroZ;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &IMUReadings, sizeof(IMUReadings));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
}
void getReadings(){
  accelX = IMU.getAccelX_mss();
  accelY = IMU.getAccelY_mss();
  accelZ = IMU.getAccelZ_mss();
  gyroX = IMU.getGyroX_rads();
  gyroY = IMU.getGyroY_rads();
  gyroZ = IMU.getGyroZ_rads();
}
