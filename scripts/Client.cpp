#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int device_id = 0;

int ENABLE_PIN = 2;
bool found = false;

int WaitTime = 100000;

enum MessageCode {ALIVE,TIMING,RESET};

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int count = 0;

void AliveResponse(){
    digitalWrite(ENABLE_PIN,HIGH);
    Serial.println(255);
    digitalWrite(ENABLE_PIN,LOW);
}

void Respond(unsigned long period){
// perpetually waiting for staring character.
    while(Serial.read()!='@'){
      continue;
    }
// If we see !, we are in the middle of another message. Return.
    if(Serial.peek()=='!'){
      Serial.read();
      return;
    }
    int response = Serial.parseInt();
    if(response!=device_id){
        return;
    }
// read in command
    response = Serial.parseInt();
    switch(response){
// Alive cascades with default (keep break out of ALIVE)
      case ALIVE:
      default:
        AliveResponse();
    }
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,LOW);
}

void loop() {
    Respond(WaitTime);
}
