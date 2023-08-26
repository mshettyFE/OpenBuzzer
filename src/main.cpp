#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

// using 1 based indexing, since atoi returns 0 for no conversion. Want to avoid this ambiguity.
bool DevicesAlive[MAX_DEVICES+1];

// Buffer for incoming message;
char buffer[bufferSize];

// Global variables to dictate weather there is a message to read or not.
// We need both since we read character by character, and thus need to know if we have read in a starting string.

// places to store timing data
uint64_t timing;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void ScanForDevices(uint64_t WaitTime){
  bool msg_start,msg_end;
  uint8_t buffer_index, received_device_id;
  uint64_t start_time,end_time;
  MessageType received_msg;
  for(int Device=1; Device<=MAX_DEVICES; Device++){
// Fire off ALIVE message for device Device
    SendMsgServer(Device,ALIVE);
// set up local timing variables
    start_time = micros();
    end_time = micros();
    received_msg = INVALID;
    received_device_id = 0;
    msg_start = false;
    msg_end = false;
    buffer_index = 0;
// wait for message for at most WaitTime microseconds
    while((end_time-start_time) < WaitTime){
      RecieveChar(buffer, buffer_index, msg_start, msg_end);
      if(msg_end && msg_start){
        ParseMsgServer(buffer,received_device_id,received_msg,timing);
        break;
      }
      end_time = micros();
    }
    ServerAction(received_msg, ALIVE, received_device_id,Device,  DevicesAlive);
  }
}

void UpdateServerDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
  for(int d=0; d<=MAX_DEVICES; ++d){
    display.printf("%d,",DevicesAlive[d]);
  }
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,HIGH);
// We set Device 0 to false. No client should have 0 as their id to avoid collision with atoi default of 0
  DevicesAlive[0] = false;
  for(int Device=1; Device <=MAX_DEVICES; Device++){
    DevicesAlive[Device] = false;
  }
}

void loop() {
  ScanForDevices(WAIT_TIME);
  UpdateServerDisplay();
}
