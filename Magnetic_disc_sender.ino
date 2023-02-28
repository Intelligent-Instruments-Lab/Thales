/*********
ESPNOW Code:

  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

 MPU6050 Code:

 MIT License

 Copyright (c) 2020 Romain JL. FETICK

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

*********/

#include <esp_now.h>
#include <WiFi.h>
#include <MPU6050_light.h>
#include "Adafruit_MLX90393.h"


Adafruit_MLX90393 sensor = Adafruit_MLX90393();
#define MLX90393_CS 10

//Start MPU6050
MPU6050 mpu(Wire);
//timer for transmission freq
unsigned long timer = 0;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0xD2, 0x14, 0x1C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
     //magnet
    int hall; 
    float hallx; 
    float hally;
    float hallz;     
       //MP6050 angle
    float ax; 
    float ay; 
    float az; 
     //MP6050 accelerometer
    float accx;
    float accy;
    float accz;
     //MP6050 gyroscope
    float gx;
    float gy;
    float gz;    
   
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Init Wire
  //Wire.begin();
   Wire.begin(SDA,SCL,(uint32_t)409600);

  // Init MPU6050
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  //while(status!=0){ } // stop everything if could not connect to MPU6050 


  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
// mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero

 if (! sensor.begin_I2C()) {       
   while (1) { delay(10); }
   
    
  }
 
 //Set gain
  sensor.setGain(MLX90393_GAIN_1X);
  
  // Set resolution, per axis
  sensor.setResolution(MLX90393_X, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_2);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_0);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  //esp_now_register_send_cb(OnDataSent);
  
  // Register peer
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

  if((millis()-timer)>5)
  
  {

 //Get MXL event, normalized to uTesla */
  sensors_event_t event;
  sensor.getEvent(&event);   
  
  //Get MPU event//
  mpu.update();

  // Set values to send
  myData.id = 2; //Set ID board number
  myData.hall = hallRead();
  myData.hallx = event.magnetic.x;
  myData.hally = event.magnetic.y;
  myData.hallz = event.magnetic.z;
  myData.ax = mpu.getAngleX();
  myData.ay = mpu.getAngleY();
  myData.az = mpu.getAngleZ();
  myData.accx = mpu.getAccX();
  myData.accy = mpu.getAccY();
  myData.accz = mpu.getAccZ();
  myData.gx = mpu.getGyroX();
  myData.gy = mpu.getGyroY();
  myData.gz = mpu.getGyroZ();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("Delivered");
  }
  else {
    Serial.println("Error sending the data");
  }
  timer = millis();
  }
}
