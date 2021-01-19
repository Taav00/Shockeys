/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */

#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include "MPU9250.h"
#include "adxl372.h"


#define SDA_PIN 25
#define SCL_PIN 26

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SERIALUSB
  #define SYS_VOL   3.3
#else
  #define SERIAL Serial
  #define SYS_VOL   5
#endif

float cali_data[3];

#define CALI_BUF_LEN           15
#define CALI_INTERVAL_TIME     250

float cali_buf[3][CALI_BUF_LEN];


ADXL372 acc;
xyz_t xyz;

BluetoothSerial ESP_BT; //Object for Bluetooth

int incoming;
unsigned long Time = 0;

MPU9250 IMU(Wire,0x68);
int status;

float deal_cali_buf(float *buf)
{
    float cali_val = 0;
    
    for(int i = 0;i < CALI_BUF_LEN;i++)
    {
        cali_val += buf[i];
    }
    cali_val = cali_val/CALI_BUF_LEN;
    return (float)cali_val;
}

void calibration(void)
{
    SERIAL.println("Please Place the module horizontally!");
    delay(1000);
    SERIAL.println("Start calibration........");
    
  
    for(int i=0;i<CALI_BUF_LEN;i++)
    {
        while (!(acc.status() & DATA_READY)); 
        acc.read(&xyz);
        cali_buf[0][i] = xyz.x;
        cali_buf[1][i] = xyz.y;
        cali_buf[2][i] = xyz.z;
        delay(CALI_INTERVAL_TIME);
        SERIAL.print('.');
    }
    SERIAL.println('.');
    for(int i=0;i<3;i++)
    {
        cali_data[i] =  deal_cali_buf(cali_buf[i]);
        if(2 == i){
      
            cali_data[i] -= 10;
        }
        SERIAL.println(cali_data[i]);
    }
    SERIAL.println("Calibration OK!!");
}
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

String timeString = ""; // holds the data to be written to the SD card

File sensorData;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup(){
    Serial.begin(115200);
    ESP_BT.begin("ESP32_Accelerometer"); //Name of your Bluetooth Signal

    Wire.begin(SDA_PIN, SCL_PIN); // sda /scl
    status = IMU.begin();

    acc.begin();
    acc.timing_ctrl(RATE_400);
    acc.measurement_ctrl(BW_200, true);
    acc.power_ctrl(MEASUREMENT_MODE);
    acc.setActiveTime(10);

    calibration();
    
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    /*listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    */
    //appendFile(SD, "/hellow.csv", "Hello ");
    //appendFile(SD, "/hellow.csv", "\n");

    readFile(SD, "/hello.csv");

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop(){
    //ReadIMU();
    RecordIMU();
    delay(5);
}

void ReadIMU(){
    IMU.readSensor();
    //ESP_BT.print("AccelX: ");
    //ESP_BT.println(IMU.getAccelX_mss(),6);
    //Serial.println("Hi serial Master");
    Serial.println(IMU.getAccelX_mss(),6);
    }

void RecordIMU(){
    
    IMU.readSensor();
    
    Time = millis();
    timeString = String(Time);
    const char *time_complete = timeString.c_str();
    dataString_0 = String(IMU.getAccelX_mss(),6);
    dataString_1 = String(IMU.getAccelY_mss(),6);
    dataString_2 = String(IMU.getAccelZ_mss(),6);
    dataString_3 = String(IMU.getGyroX_rads(),6);
    dataString_4 = String(IMU.getGyroY_rads(),6);
    dataString_5 = String(IMU.getGyroZ_rads(),6);

    if (acc.status() & DATA_READY) {
        acc.read(&xyz);
        dataString_6 = String((xyz.x - cali_data[0]) / 10.0);
        dataString_7 = String((xyz.y - cali_data[1]) / 10.0);
        dataString_8 = String((xyz.z - cali_data[2]) / 10.0);
    }
    Data = timeString + ","+ dataString_0 +  "," + dataString_1 + "," + dataString_2 + "," + dataString_3  + "," + dataString_4 + "," + dataString_5 + "," + dataString_6 + "," + dataString_7 + "," + dataString_8 + "\n";

    const char *Data_full = Data.c_str();
    /*const char *data_complete_1 = dataString_1.c_str();
    const char *data_complete_2 = dataString_2.c_str();
    const char *data_complete_3 = dataString_3.c_str();
    const char *data_complete_4 = dataString_4.c_str();
    const char *data_complete_5 = dataString_5.c_str();
    const char *data_complete_6 = dataString_6.c_str();
    const char *data_complete_7 = dataString_7.c_str();
    const char *data_complete_8 = dataString_8.c_str();*/
    appendFile(SD, "/hello.csv", Data_full);
    /*appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_0);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_1);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_2);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_3);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_4);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_5);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_6);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_7);
    appendFile(SD, "/hello.csv", ",");
    appendFile(SD, "/hello.csv", data_complete_8);
    appendFile(SD, "/hello.csv", "\n");*/
    }
