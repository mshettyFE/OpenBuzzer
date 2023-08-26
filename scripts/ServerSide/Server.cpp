#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

// using 1 based indexing, since atoi returns 0 for no conversion. Want to avoid this ambiguity.
bool DevicesAlive[MAX_DEVICES+1];

// Rankings of players
int RankingIndex = 0;
uint8_t Rankings[MAX_DEVICES];

// Buffer for incoming message;
char buffer[bufferSize];

uint64_t synchronized_start_time = 0;

// places to store timing data
uint64_t timing;

// Reset flag
bool reset_flag  = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void IRAM_ATTR TogglePressed(){
 reset_flag = true; 
}

bool ServerAction(MessageType rec_msg, MessageType exp_msg, uint8_t rec_device_id, uint8_t exp_device_id, uint64_t timing){
  if(rec_msg!=exp_msg && rec_device_id!= exp_device_id){
    switch(exp_msg){
      case ALIVE:
        DevicesAlive[exp_device_id] = false;
        break;
      case TIMING:
      case LOCK_IN:
      case RESET:
      case INVALID:
      default:
        break;
    }
    return false;
  }

  switch(rec_msg){
    case ALIVE:
      DevicesAlive[rec_device_id] = true;
// We assign the latest start 
      if(timing> synchronized_start_time){
        synchronized_start_time = timing;
      }
      return true;
      break;
    case TIMING:
      break;
    case LOCK_IN:
    case RESET:
    case INVALID:
    default:
      break;
  }
  return false;
}


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
    ServerAction(received_msg, ALIVE, received_device_id,Device,timing);
  }
}

void ResetDevices(uint64_t WaitTime){
  ScanForDevices(WaitTime);
  for(int Device=1; Device<=MAX_DEVICES; Device++){
    if(DevicesAlive[Device]){
  // Fire off RESET message for device Device. It is the Client's problem to receive the message
      SendMsgServer(Device,RESET);
    }
  }
// clear rankings
  for(int Device=0; Device<MAX_DEVICES; Device++){
    Rankings[Device] = 0;
  }
// clean out priority queue (TODO)
}

void UpdateServerDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
  for(int d=0; d<=MAX_DEVICES; ++d){
    display.printf("%d,",DevicesAlive[d]);
  }
  display.printf("Offset:%llu\n",synchronized_start_time);
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
  pinMode(TRIGGER_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), TogglePressed, FALLING);
// We set Device 0 to false. No client should have 0 as their id to avoid collision with atoi default of 0
  DevicesAlive[0] = false;
  for(int Device=1; Device <=MAX_DEVICES; Device++){
    DevicesAlive[Device] = false;
  }
// Scan all devices to check ALIVE connections and get global offset time
  ScanForDevices(WAIT_TIME);
}

void loop() {
  ScanForDevices(WAIT_TIME);
  UpdateServerDisplay();
  if(reset_flag){
    ResetDevices(WAIT_TIME);
    reset_flag = false;
  }
}
