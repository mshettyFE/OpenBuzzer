#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"

const int DEVICE_ID  = 1;

unsigned long long start, end;

// ENABLE_PIN to toggle between transmit and receive
int ENABLE_PIN = 2;

// Buffer for incoming message;
byte buffer_index = 0;
char buffer[bufferSize];

// Global variables to dictate weather there is a message to read or not.
// We need both since we read character by character, and thus need to know if we have read in a starting string.
bool msg_start = false;
bool msg_end = false;

// places to store timing data
uint64_t timing;
MessageType MSGT;

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

bool ParseCharacterClient(){
// Assumes message has the following form:
// !%d#%d@ where the first %d is the device ID and second %d is device type
  char* endIndex = strtok(buffer,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    return false;
  }
  int cur_device = atoi(endIndex);
  if(cur_device!=DEVICE_ID){
    MSGT = INVALID;
    return false;
  }
  endIndex = strtok(NULL, delimiter);
// we need to know what the server wants from the client. Hence, we need a valid message type
  if(endIndex==NULL){
    return false;
  }
  int temp = atoi(endIndex);
  if(temp<ALIVE || temp > RESET){
    return false;
  }
  switch(temp){
    case ALIVE:
      MSGT = ALIVE;
      break;
    case TIMING:
      MSGT = TIMING;
      break;
    case RESET:
      MSGT = RESET;
      break;
    case INVALID:
    default:
      MSGT = INVALID;
      return false;
  }
}


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,LOW);
  start = micros();
}

void loop() {
}
