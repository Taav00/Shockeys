/*
Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#include "communication_helmet.h"
#include "communication_app.h"

#include <WiFi.h>
 
void setup() {
	
	// Init Serial Monitor
	Serial.begin(115200);

  // Setup router as BLE server (peripheral) to communicate with app
  setupBLE();

	// Setup router as slave to communicate with helmet using ESP-NOW
	setupESPNOW();


}

void loop() {
	
}
