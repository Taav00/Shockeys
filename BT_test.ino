/*Program to get accelerometer data from mobile ESP32 using Serial Bluetooth
 */

#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include <Wire.h>
#include "MPU9250.h"

BluetoothSerial ESP_BT; //Object for Bluetooth

int incoming;

MPU9250 IMU(Wire,0x68);
int status;

void setup() {
    Serial.begin(9600); //Start Serial monitor in 9600
    ESP_BT.begin("ESP32_LED_Control"); //Name of your Bluetooth Signal
    Serial.println("Bluetooth Device is Ready to Pair");
  
    Wire.begin();
    status = IMU.begin();

    if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    }
    
    pinMode (5, OUTPUT);//Specify that LED pin is output
}

void loop() {
    ReadIMU();
    if (ESP_BT.available()) //Check if we receive anything from Bluetooth
    {
      incoming = ESP_BT.read(); //Read what we recevive 
      Serial.print("Received:"); Serial.println(incoming);

      if (incoming == 49)
          {
          digitalWrite(5, HIGH);
          ESP_BT.println("LED turned ON");
          }

      if (incoming == 48)
          {
          digitalWrite(5, LOW);
          ESP_BT.println("LED turned OFF");
          }  
      if(incoming == 50)
      {
          while(incoming = 50)
          {ReadIMU();
  }
      }   
    }
    delay(20);
}

void ReadIMU(){
    IMU.readSensor();
    ESP_BT.print("AccelX: ");
    ESP_BT.println(IMU.getAccelX_mss(),6);

    }
