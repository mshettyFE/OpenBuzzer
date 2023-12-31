// https://forum.arduino.cc/t/serial-input-basics-updated/382007/3
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"

uint64_t timing = micros();

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
// Set up OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
// Start Serial
  Serial.begin(115200);
// Set ENABLE High to enable transmission
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,HIGH);
}

void loop() {
// Send number
timing = micros();
  Serial.printf("!%llu@",timing);
// Display current number

  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Count:%llu",timing);
  display.display();
}
