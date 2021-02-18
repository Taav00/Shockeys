/*

Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

Definition: Executes setup for the helmet.
*/

#ifndef INITIALIZATIONHELMET_H_
#define INITIALIZATIONHELMET_H_

#define SDA_PIN     25
#define SCL_PIN     26

void initializationHelmet();
void readISR();

bool checkDataCore0();
bool checkDataCore1();

void resetDataCore0();
void resetDataCore1();

#endif // INITIALIZATIONHELMET_H_
