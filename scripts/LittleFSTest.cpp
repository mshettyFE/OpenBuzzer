/*********
Directly copied from https://RandomNerdTutorials.com/esp8266-nodemcu-vs-code-platformio-littlefs/  
Original author Rui Santos
*********/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LittleFS.h"
 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int ENABLE_PIN = 2;
bool found = false;
int incomingByte = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);

  if(!LittleFS.begin()){
    display.println("An Error has occurred while mounting LittleFS");
    display.display();
    return;
  }
  
  File file = LittleFS.open("/test.txt", "r");
  if(!file){
    display.setCursor(0, 0);
    display.println("Failed to open file for reading");
    display.display();
    return;
  }
  
  display.setCursor(0, 0);
  while(file.available()){
    display.write(file.read());
  }
  display.display();
}
 
void loop() {

}
