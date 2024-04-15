/*********
  This instantiates an ESPNow Wifi connection from one Thales controller to the receiver unit.
  Upload as is on the first controller (left hand). When uploading on the second controller change the board ID to 2 (line 162).

  Code is based on Rui Santos ESPNow project at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_BNO055.h>
#include "Adafruit_MLX90393.h"



Adafruit_MLX90393 sensor = Adafruit_MLX90393();
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
#define MLX90393_CS 10


//timer for transmission freq
unsigned long timer = 0;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x94, 0xB9, 0x7E, 0xD2, 0x14, 0x1C};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
     //magnet
    float hallx; 
    float hally;
    float hallz;     
       //BNO055 angle
    float anglex; 
    float angley; 
    float anglez; 
     //BNO055accelerometer
    float accx;
    float accy;
    float accz; 
    int battery;
   
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

  analogReadResolution(12);

 if (!bno.begin())
  {
    while (1); { delay(10); }
  }

 if (! sensor.begin_I2C()) {       
   while (1) { delay(10); }  
    
  }

 delay(1000);

/* Use external crystal for better accuracy */
bno.setExtCrystalUse(true);
 
//Set gain
sensor.setGain(MLX90393_GAIN_1X);
  
// Set resolution, per axis
sensor.setResolution(MLX90393_X, MLX90393_RES_19);
sensor.setResolution(MLX90393_Y, MLX90393_RES_19);
sensor.setResolution(MLX90393_Z, MLX90393_RES_19);


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

  int Voltout = analogRead(A2);
  Voltout = Voltout * 5000 / 4096; //read battery charge

 //Get MXL event, normalized to uTesla */
  sensors_event_t event, orientationData, linearAccelData;
  sensor.getEvent(&event); 
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);  


  // Set values to send
  myData.id = 1; //Set ID board number
  myData.hallx = event.magnetic.x;
  myData.hally = event.magnetic.y;
  myData.hallz = event.magnetic.z;
  myData.anglex = orientationData.orientation.x;
  myData.angley = orientationData.orientation.y;
  myData.anglez = orientationData.orientation.z;
  myData.accx = linearAccelData.orientation.x;
  myData.accy = linearAccelData.orientation.y;
  myData.accz = linearAccelData.orientation.z;
  myData.battery = Voltout;

  
 
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
