#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Possible Messsages that can be sent. ALIVE is used to register a device. TIMING is used to get current buzzin time of device, and RESET is used to clear state of devices
enum MessageType {INVALID,ALIVE,TIMING,RESET};

// Maximum number of devices that can be connected
const int MAX_DEVICES = 32;
// using 1 based indexing, since atoi returns 0 for no conversion. Want to avoid this ambiguity.
bool DevicesAlive[MAX_DEVICES+1];
// Response wait time in microseconds
const unsigned long long WAIT_TIME = 10000000;

// ENABLE_PIN to toggle between transmit and receive
int ENABLE_PIN = 2;

// Start and end characters for messages
char start_marker = '!';
char end_marker = '@';
// Delimiter to seperate different values within a message
const char delimiter[2] = "#";

// Buffer for incoming message;
const byte bufferSize = 32;
byte buffer_index = 0;
char buffer[bufferSize];

// Global variables to dictate weather there is a message to read or not.
// We need both since we read character by character, and thus need to know if we have read in a starting string.
bool msg_start = false;
bool msg_end = false;

// places to store timing data
uint64_t timing;
MessageType MSGT;
int device;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void RecieveChar(){
// read in a single character from the bus if available
  char rc;
  while(Serial.available() > 0 ){
    rc = Serial.read();
//    Serial.printf("%c",rc);
    if(!msg_start){
          if(rc==start_marker){
//            Serial.println("start");
            // Point to beginning of array, and allow future reads of Serial to be processed
            buffer_index = 0;
            msg_start = true;
          }
    }
    else{
      if(rc!=end_marker){
// read in characters, then advance index
//            Serial.println("mid");
        buffer[buffer_index] = rc;
        buffer_index++;
// If the index exceeds the buffer size,t then we have gone on for too long. set msg_start and msg_end to false, reset buffer to start and return
        if(buffer_index>=bufferSize){
          msg_start = false;
          msg_end = false;
          buffer_index = 0;
          return;
        }
      }
      else{
// We are at the end of the message. Need to null terminate string
//            Serial.println("end");
        buffer[buffer_index] = '\0';
        msg_end = true;
// exit to start parsing message
        return ;
      }
    }
  }
}

bool ParseCharacter(){
// Assumes message has one of two forms:
// !%d#%d@ where the first %d is the message type and second %d is device ID
// !%d#%d#%llu@ where first %d is the message type equal to TIMING, second %d is device ID, %, and %llu is an unsigned long long
  char* endIndex = strtok(buffer,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    return false;
  }

// First variable should be the MessageType. Since the enum is contiguous, we can check if atoi() returns something below or above valid range of enum
  int temp = atoi(endIndex); 
// invalid message type
  if(temp<ALIVE || temp>RESET){
    MSGT = INVALID;
    return false;
  }
  else{
    switch(temp){
      case ALIVE:
        MSGT = ALIVE;
        break;
      case RESET:
        MSGT = RESET;
        break;
      case TIMING:
        MSGT = TIMING;
        break;
      case INVALID:
      default:
        MSGT = INVALID;
        return false;
    }
  }

// Grab next part
  endIndex = strtok(NULL, delimiter);
  if(endIndex==NULL){
    return false;
  }

// Switch on what MSGT is. 
  switch(MSGT){
    case ALIVE:
      device = atoi(endIndex);
      if(device<1 || device > MAX_DEVICES){
        return false;
      }
      return true;
      break;
    case RESET:
      device = atoi(endIndex);
      if(device<1 || device > MAX_DEVICES){
        return false;
      }
      return true;
      break;
    case TIMING:
      endIndex = strtok(NULL, delimiter);
      if(endIndex==NULL){
        return false;
      }
      timing = strtoull(endIndex,NULL,10);
      if(errno==ERANGE){
        return false;
      }
      return true;
      break;
// Invalid and default cascade into each other
    case INVALID:
    default:
      return false;
      break;
  }
// Just in case, return false. Shouldn't ever get here, but you never know.
  return false;
}

void ScanForDevices(unsigned long long WaitTime){
  for(int Device=1; Device<MAX_DEVICES+1; Device++){
// Fire off ALIVE message for device Device
    Serial.printf("!%d#%d@",ALIVE,Device);
// Disable transmission, enable receive
    digitalWrite(ENABLE_PIN,LOW);
// set up timing variables
    unsigned long long start_time = micros();
    unsigned long long end_time = micros();
    bool valid_msg = false;
// wait for message for at most WaitTime us
    while((end_time-start_time) < WaitTime){
      RecieveChar();
      if(msg_end && msg_start){
        msg_start = false;
        msg_end = false;
        valid_msg = ParseCharacter();
        break;
      }
      end_time = micros();
    }
    if(valid_msg && (MSGT==ALIVE) && (Device==device)){
      DevicesAlive[Device] = true;      
    }
    else{
      DevicesAlive[Device] = false;
    }
    digitalWrite(ENABLE_PIN,HIGH);
  }
}

void PrintAlive(){
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
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,HIGH);
  DevicesAlive[0] = false;
  for(int Device=1; Device <=MAX_DEVICES; Device++){
    DevicesAlive[Device] = false;
  }
}

void loop() {
  ScanForDevices(WAIT_TIME);
  PrintAlive();
//  while(1){}
}
