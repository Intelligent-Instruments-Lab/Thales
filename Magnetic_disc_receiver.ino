/*********
  This is code instantiates one ESPNow Wifi receiver with the following custom MAC address: 0x94, 0xB9, 0x7E, 0xD2, 0x14, 0x1C
  It matches the sender code on this repo for a total of two Thales controllers.
  Works on any ESP32 board viare serial.
  
  Code is based on Rui Santos EspNow projectat https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Set your new MAC Address
uint8_t newMACAddress[] = {0x94, 0xB9, 0x7E, 0xD2, 0x14, 0x1C};

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int id; // must be unique for each sender board
     //
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
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board (now we have two)
struct_message board1;
struct_message board2;
//struct_message board3;

// Create an array with all the structures (now we have two)
struct_message boardsStruct[2] = {board1, board2};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  
  //Get peer MAC address
  //char macStr[18];
  //Serial.print("Packet received from: ");
  //snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.println(macStr);

  //Copy content incomingData into myData variable
  memcpy(&myData, incomingData, sizeof(myData));

  
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].hallx = myData.hallx;
  boardsStruct[myData.id-1].hally = myData.hally;
  boardsStruct[myData.id-1].hallz = myData.hallz;
  boardsStruct[myData.id-1].anglex = myData.anglex;
  boardsStruct[myData.id-1].angley = myData.angley;
  boardsStruct[myData.id-1].anglez = myData.anglez;
  boardsStruct[myData.id-1].accx = myData.accx;
  boardsStruct[myData.id-1].accy = myData.accy;
  boardsStruct[myData.id-1].accz = myData.accz;
  boardsStruct[myData.id-1].battery = myData.battery;




  Serial.printf("%u ", myData.id);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].hallx);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].hally);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].hallz);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].anglex);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].angley);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].anglez);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].accx);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].accy);
  Serial.printf("%.2f ", boardsStruct[myData.id-1].accz);
  Serial.printf("%u\r ", boardsStruct[myData.id-1].battery);

}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

 esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  // Acess the variables for each board
  /*int board1X = boardsStruct[0].x;
  int board1Y = boardsStruct[0].y;
  int board2X = boardsStruct[1].x;*/

}
