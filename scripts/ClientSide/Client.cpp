#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Constants.h"
#include "SerialParsing.h"

const int DEVICE_ID  = 2;

bool Pressed = false;

int count = 0;

bool this_buzzer_locked_in = false;

bool msg_start,msg_end;
uint8_t buffer_index, received_device_id;
MessageType received_msg;

uint64_t buzz_in_time;
// Buffer for incoming message;
char buffer[bufferSize];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void UpdateClientDisplayDebug(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Device ID:%d\n",DEVICE_ID);
  display.printf("Pressed:%llu\n",buzz_in_time);
  display.printf("Locked In:%d\n",this_buzzer_locked_in);
  display.printf("%d\n",count++);
  display.display();
}

bool ClientAction(MessageType rec_msg, int rec_device_id){
  if( rec_device_id!= DEVICE_ID){
    return false;
  }
  switch(rec_msg){
    case ALIVE:
    {
      SendMsgClient(DEVICE_ID,ALIVE,micros());
      break;
    }
    case RESET:
    {
      buzz_in_time = 0;
      Pressed = false;
      this_buzzer_locked_in = false;
      SendMsgClient(DEVICE_ID,RESET,0);
      break;
    }
    case TIMING:
    {
      SendMsgClient(DEVICE_ID,TIMING,buzz_in_time);
      break;
    }
    case LOCK_IN:
    {
      SendMsgClient(DEVICE_ID,LOCK_IN,0);
      this_buzzer_locked_in = true;
      Serial.printf("\n\nLockedIn\n\n");
      delay(1000);
      break;
    }
    case INVALID:
    default:
    {
      SendMsgClient(DEVICE_ID,INVALID,0);
      break;
    }
  }
  return true;
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
  bool valid = false;
  while(1){
    UpdateClientDisplayDebug();
    ReceiveChar(buffer,buffer_index,msg_start,msg_end);
    if(msg_end && msg_start){
      valid = ParseMsgClient(buffer,received_device_id,received_msg);
// WE got a valid message from the server. Perform an action
      if(valid){
        ClientAction(received_msg,received_device_id);
      }
// We got an invalid message. send an INVALID response
      else{
        ClientAction(INVALID,DEVICE_ID);
      }
      received_msg = INVALID;
      received_device_id = INVALID_DEVICE;
      msg_start = false;
      msg_end = false;
      buffer_index = 0;
      valid = true;
      break;
    }
  }
}

void setup() {
  buzz_in_time = 0;
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
  received_device_id = INVALID_DEVICE;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
}

void loop() {
  ScanForCommands();
}
