#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int ENABLE_PIN = 2;

char start_marker = '!';
char end_marker = '@';

const byte bufferSize = 32;
byte buffer_index = 0;
char buffer[bufferSize];

bool msg_start = false;
bool msg_end = false;

uint64_t data=0;


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void RecieveChar(){
  char rc;
  while(Serial.available() >0 ){
    rc = Serial.read();
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
// If the index exceeds the buffer size, set buffer_index to end of buffer
        if(buffer_index>=bufferSize){
          buffer_index = bufferSize-1;
        }
      }
      else{
// We are at the nd of the message. Need to null terminate string
//            Serial.println("end");
        buffer[buffer_index] = '\0';
        msg_end = true;
      }
    }
  }
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.println("Start");
  display.display();
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,LOW);
}

void loop() {
  RecieveChar();
  if(msg_end&&msg_start){
    msg_start = false;
    msg_end = false;
    data = strtoull(buffer,NULL,10);
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Count:%ull",data);
    display.display();
  }
}
