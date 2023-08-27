#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

const int DEVICE_ID  = 3;

bool Pressed = false;

int count = 0;

bool locked_in = false;

bool msg_start,msg_end;
uint8_t buffer_index, received_device_id;
MessageType received_msg;

uint64_t buzz_in_time =0;
// Buffer for incoming message;
char buffer[bufferSize];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void UpdateClientDisplayDebug(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.printf("Pressed:%llu\n",buzz_in_time);
  display.printf("Locked In:%d\n",locked_in);
  display.printf("%d\n",count++);
  display.display();
}

bool ClientAction(MessageType rec_msg, uint8_t rec_device_id, uint8_t exp_device_id){
  if( rec_device_id!= exp_device_id){
    return false;
  }
  switch(rec_msg){
    case ALIVE:
      SendMsgClient(rec_device_id,ALIVE,micros());
      return true;
      break;
    case RESET:
      buzz_in_time = 0;
      Pressed = false;
      locked_in = false;
      SendMsgClient(rec_device_id,RESET,0);
      return true;
      break;
    case TIMING:
      SendMsgClient(rec_device_id,TIMING,buzz_in_time);
      return true;
      break;
    case LOCK_IN:
      SendMsgClient(rec_device_id,LOCK_IN,0);
      locked_in = true;
      return true;
      break;
    case INVALID:
    default:
      SendMsgClient(rec_device_id,INVALID,0);
      break;
  }
  return false;
}

void IRAM_ATTR TooglePressed(){
  if(!Pressed){
    Pressed = true;
    buzz_in_time = micros();
//    Serial.printf("%dn",Pressed);
//    Serial.printf("%llu\n",buzz_in_time);
  }
}

void ScanForCommands(){
  while(1){
    UpdateClientDisplayDebug();
    RecieveChar(buffer,buffer_index,msg_start,msg_end);
    if(msg_end && msg_start){
      ParseMsgClient(buffer,received_device_id,received_msg);
      ClientAction(received_msg,received_device_id,DEVICE_ID);
      received_msg = INVALID;
      received_device_id = 0;
      msg_start = false;
      msg_end = false;
      buffer_index = 0;
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
  display.printf("Pressed:%llu\n",buzz_in_time);
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
}

void loop() {
  ScanForCommands();
}
