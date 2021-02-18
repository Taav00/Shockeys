/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Definition: Executes setup for the helmet.
*/

#include "initializationHelmet.h"
#include "sensorsHelmet.h"
#include "SDHelmet.h"
#include "communication.h"
#include "Arduino.h"


// flags indicating whether new readings have to be processed
bool dataCore0 = false;
bool dataCore1 = false;

void initializationHelmet()
{
	 // start both IMUs and th big accelerometer with matching I2C buses
	 setup_sensors();

 	 // SD card initialization
 	 SDCardSetup();
    
   // set-up ESP-NOW
   setupESPNOW();
}


void readISR()
{
  if (dataCore0){
    Serial.println("Warning: Sensors readings are being updated even though Core0 didn't process the previous ones.");
  }

  if(dataCore1){
    Serial.println("Warning: Sensors readings are being updated even though Core1 didn't process the previous ones.");
  }

  Serial.println("In ISR :).");

  // update readings from sensors 
  read_sensors();
  

  // notify both cores that new readings are ready to be processed
  dataCore0 = true;
  dataCore1 = true;
}

bool checkDataCore0(){
  return dataCore0;
}

bool checkDataCore1(){
  return dataCore1;
}

void resetDataCore0(){
  dataCore0 = false;
}

void resetDataCore1(){
  dataCore1 = false;
}
