/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

// Define variables to store incoming readings
float incomingAccX;
float incomingAccY;
float incomingAccZ;
float incomingGyroX;
float incomingGyroY;
float incomingGyroZ;

//Structure example to receive data
typedef struct struct_message {
    float accX;
    float accY;
    float accZ;
    float gyrX;
    float gyrY;
    float gyrZ;
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incomingAccX = incomingReadings.accX;
  incomingAccY = incomingReadings.accY;
  incomingAccZ = incomingReadings.accZ;
  incomingGyroX = incomingReadings.gyrX;
  incomingGyroY = incomingReadings.gyrY;
  incomingGyroZ = incomingReadings.gyrZ;
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  //memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  updateDisplay();
  delay(10000);
}

void updateDisplay(){
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Acceleration in X: ");
  Serial.print(incomingReadings.accX);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Y: ");
  Serial.print(incomingReadings.accY);
  Serial.println(" m/s^2");
  Serial.print("Acceleration in Z: ");
  Serial.print(incomingReadings.accZ);
  Serial.println(" m/s^2");
  Serial.print("Gyroscope in X: ");
  Serial.print(incomingReadings.gyrX);
  Serial.println(" rad/s^2");
  Serial.print("Gyroscope in Y: ");
  Serial.print(incomingReadings.gyrY);
  Serial.println(" rad/s^2");
  Serial.print("Gyroscope in Z: ");
  Serial.print(incomingReadings.gyrZ);
  Serial.println(" rad/s^2");
  Serial.println();
}
