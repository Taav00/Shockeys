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


#ifndef COMMUNICATION_APP_H_
#define COMMUNICATION_APP_H_


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Setup a BLE server (= peripheral device)
void setupBLE();

// Send sensors data to app
void sendDataBLE(char* sensors_readings);

#endif // COMMUNICATION_APP_H_
