#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

const int DEVICE_ID  = 2;

bool msg_start,msg_end;
uint8_t buffer_index, received_device_id;
MessageType received_msg;

uint64_t start;
uint64_t delta = 525600;
// Buffer for incoming message;
char buffer[bufferSize];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.display();
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,LOW);
  received_msg = INVALID;
  received_device_id = 0;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
  start = micros();
}

void ResetClient(){
  digitalWrite(ENABLE_PIN,LOW);
  received_msg = INVALID;
  received_device_id = 0;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
}


void ScanForCommands(){
  while(1){
    RecieveChar(buffer,buffer_index,msg_start,msg_end);
    if(msg_end && msg_start){
     ParseMsgClient(buffer,received_device_id,received_msg);
     ClientAction(received_msg,received_device_id,DEVICE_ID);
     ResetClient();
     break;
    }
  } 
}

void UpdateClientDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.display();

}

void loop() {
  ScanForCommands();
  UpdateClientDisplay();
}
