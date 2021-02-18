/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Description: Implements all functions, variables, structures and constants needed
to communicate in between the router and the app using BLE.
*/


#include "communication_app.h"

#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

static BLECharacteristic *pCharacteristic;

// Setup a BLE server (= peripheral device)
void setupBLE() {
  Serial.println("Starting BLE work!");

  BLEDevice::init("Check-y Router");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());
  
  pCharacteristic->setValue("Hello World says Checky");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

// Send sensors data to app
void sendDataBLE(char* sensors_readings) {
  pCharacteristic->setValue(sensors_readings);
  pCharacteristic->notify();
}
