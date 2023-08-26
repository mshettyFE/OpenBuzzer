#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

const int DEVICE_ID  = 2;

bool Pressed = false;

bool msg_start,msg_end;
uint8_t buffer_index, received_device_id;
MessageType received_msg;

uint64_t start;
uint64_t delta =0;
// Buffer for incoming message;
char buffer[bufferSize];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool ClientAction(MessageType rec_msg, uint8_t rec_device_id, uint8_t exp_device_id){
  if( rec_device_id!= exp_device_id){
    return false;
  }
  switch(rec_msg){
    case ALIVE:
      SendMsgClient(rec_device_id,rec_msg);
      return true;
      break;
    case RESET:
      delta = 0;
      Pressed = false;
      break;
    case TIMING:
      break;
    case INVALID:
    default:
      break;
  }
  return false;
}

void ResetClient(){
  digitalWrite(ENABLE_PIN,LOW);
  received_msg = INVALID;
  received_device_id = 0;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
}

void IRAM_ATTR TooglePressed(){
  if(!Pressed){
    Pressed = true;
    delta = micros()-start;
    Serial.printf("%dn",Pressed);
    Serial.printf("%llu\n",delta);
  }
}

void UpdateClientDisplay(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.printf("Pressed:%llu\n",delta);
  display.display();
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

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.printf("Pressed:%llu\n",delta);
  display.display();
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  pinMode(TRIGGER_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), TooglePressed, FALLING);
  digitalWrite(ENABLE_PIN,LOW);
  received_msg = INVALID;
  received_device_id = 0;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
  start = micros();
}

void loop() {
  ScanForCommands();
    UpdateClientDisplay();

}
