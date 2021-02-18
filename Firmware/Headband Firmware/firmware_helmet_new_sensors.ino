/*
Author: Diane Marquette
Last modification: January 2021 
  
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
  
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*/

#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include "sensorsHelmet.h"
#include "initializationHelmet.h"
#include "SDHelmet.h"
#include "FIFO.h"

#define SAFETY_THRESH                   0   // to be chosen

TaskHandle_t TaskSend;
TaskHandle_t TaskStore;

unsigned long Time = 0;

// Create a struct_message to hold sensor readings
struct_message readings;

struct_message dataToSend;

// FIFO buffer to avoid losing data if connection is lost
FIFO myBuffer;

// Variable to know if it's the 1st data packet that we send
bool firstData = true;

String Data = "";
String dataString_0 = ""; // holds the data to be written to the SD card
String dataString_1 = ""; // holds the data to be written to the SD card
String dataString_2 = ""; // holds the data to be written to the SD card

String dataString_3 = ""; // holds the data to be written to the SD card
String dataString_4 = ""; // holds the data to be written to the SD card
String dataString_5 = ""; // holds the data to be written to the SD card

String dataString_6 = ""; // holds the data to be written to the SD card
String dataString_7 = ""; // holds the data to be written to the SD card
String dataString_8 = ""; // holds the data to be written to the SD card

String dataString_9 = ""; // holds the data to be written to the SD card
String dataString_10 = ""; // holds the data to be written to the SD card
String dataString_11 = ""; // holds the data to be written to the SD card

String dataString_12 = ""; // holds the data to be written to the SD card
String dataString_13 = ""; // holds the data to be written to the SD card
String dataString_14 = ""; // holds the data to be written to the SD card
String timeString = ""; // holds the data to be written to the SD card


void setup() {

  Serial.begin(115200);
  //ESP_BT.begin("ESP32_Accelerometer"); //Name of your Bluetooth Signal

  initializationHelmet();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    TaskSendCode,   /* Task function. */
    "TaskSend",     /* name of task. */
    10000,          /* Stack size of task */
    NULL,           /* parameter of the task */
    5,              /* priority of the task */
    &TaskSend,      /* Task handle to keep track of created task */
    0);             /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    TaskStoreCode,   /* Task function. */
    "TaskStore",     /* name of task. */
    10000,           /* Stack size of task */
    NULL,            /* parameter of the task */
    5,               /* priority of the task */
    &TaskStore,      /* Task handle to keep track of created task */
    1);              /* pin task to core 1 */
  delay(500);

}

//TaskSendCode: sends large accelerations to router
void TaskSendCode( void * pvParameters ) {

  for (;;) {
    
    //Pop items off the FIFO buffer until it's empty
    while (myBuffer.size() > 0) {

      // Only update  the values to be sent if the last data package has
      // been successfully sent or it's the first data packet that we send
      if ((checkSuccess() == "Delivery Success :)") || firstData) {

        firstData = false;
        dataToSend = myBuffer.pop();

      }

      // Send message via ESP-NOW
      const uint8_t *peer_addr = slave.peer_addr;
      esp_err_t result = esp_now_send(peer_addr, (uint8_t *) &dataToSend, sizeof(dataToSend));

      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        // Serial.println("Error sending the data");
      }

      vTaskDelay(1);

    }
    
  }

}

//TaskStoreCode: stores sensors readings on SD card
void TaskStoreCode( void * pvParameters ) {

  for (;;) {

    readISR(); // called here because interrupt isn't working 

    // if new readings are available AND
    // at least one of the sensors measures a linear acceleration above our safety threshold
    if ( checkDataCore0() &&
         ((accel_1.getAccelX_mss() > SAFETY_THRESH) || (accel_1.getAccelY_mss() > SAFETY_THRESH) || (accel_1.getAccelZ_mss() > SAFETY_THRESH) ||
          (accel_2.getAccelX_mss() > SAFETY_THRESH) || (accel_2.getAccelY_mss() > SAFETY_THRESH) || (accel_2.getAccelZ_mss() > SAFETY_THRESH) ||
          (getBigAccX() > SAFETY_THRESH) || (getBigAccY() > SAFETY_THRESH) || (getBigAccZ() > SAFETY_THRESH)) ) {

      // copy new readings into a structure
      readings = copy_readings(readings);
            
      // Serial.println(xyz.x);
      
      // push readings to be sent above onto the FIFO buffer
      myBuffer.push(readings);

      // reset flag to indicate the new readings were processed by Core0
      resetDataCore0();

    }

    
    // if new readings are available
    // and they were already processed by Core0
    if (checkDataCore1()) {

      Time = millis();
      timeString = String(Time);
      const char *time_complete = timeString.c_str();

      // data from IMU 1
      dataString_0 = String(accel_1.getAccelX_mss(), 6);
      dataString_1 = String(accel_1.getAccelY_mss(), 6);
      dataString_2 = String(accel_1.getAccelZ_mss(), 6);
      dataString_3 = String(gyro_1.getGyroX_rads(), 6);
      dataString_4 = String(gyro_1.getGyroY_rads(), 6);
      dataString_5 = String(gyro_1.getGyroZ_rads(), 6);

      // data from IMU 2
      dataString_6 = String(accel_2.getAccelX_mss(), 6);
      dataString_7 = String(accel_2.getAccelY_mss(), 6);
      dataString_8 = String(accel_2.getAccelZ_mss(), 6);
      dataString_9 = String(gyro_2.getGyroX_rads(), 6);
      dataString_10 = String(gyro_2.getGyroY_rads(), 6);
      dataString_11 = String(gyro_2.getGyroZ_rads(), 6);

      // data from big accelerometer
      dataString_12 = String(getBigAccX());
      dataString_13 = String(getBigAccY());
      dataString_14 = String(getBigAccZ());
      

      Data = timeString + "," + dataString_0 +  "," + dataString_1 + ","
             + dataString_2 + "," + dataString_3  + "," + dataString_4 + ","
             + dataString_5 + "," + dataString_6 + "," + dataString_7 + ","
             + dataString_8 + "," + dataString_9 + "," + dataString_10 + ","
             + dataString_11 + "," + dataString_12 + "," + dataString_13 + ","
             + dataString_14 + "\n";

      const char *Data_full = Data.c_str();

      appendFile(SD, "/newdata.csv", Data_full);

      // reset flag to indicate the new readings were processed by Core1
      resetDataCore1();
    }
    yield();

    delay(1000);
  }
  
  

}

void loop() {

  // make loopTask pinned to CPU1 idle
  // (avoids that TaskStoreCode (which is also pinned to CPU1) to be starved of CPU time)
  vTaskDelay(portMAX_DELAY);

}
