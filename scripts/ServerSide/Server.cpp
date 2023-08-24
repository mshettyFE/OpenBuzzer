#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int ENABLE_PIN = 2;

int WaitTime = 100000;

enum MessageCode {ALIVE,TIMING,RESET};

// Array of player IDs RS485 supports a max of 32 devices
// 0 means device not present.
// 1 means device present
bool AvailablePlayers[32] = {0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0};

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long start_time = micros();

void ScanForDevice(unsigned long period){
    int response;
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Starting Scan...");
    display.display();
    for(int Device=0; Device<32; ++Device){
        Serial.printf("@%d@!%d!",Device,ALIVE );
// expects 255 as response
        digitalWrite(ENABLE_PIN,LOW);
        unsigned long start = micros();
        unsigned long end = micros();
        bool device_found = false;
        while((end-start) < period){
          if(Serial.read() == '('){
            response = Serial.parseInt();
            if(Serial.read()==')'){
              if(response==255){
                device_found=true;
                display.println(Device);
                display.display();
                break;
              }
            }
          }
          end = micros();
        }
        if(device_found){
            AvailablePlayers[Device] = true;
        }
        digitalWrite(ENABLE_PIN,HIGH);
    }
    display.println("Scan Done");
    display.display();
    delay(2000);
}

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
    ScanForDevice(WaitTime);
// Display current number
/*
  bool f  = false;
  display.clearDisplay();
  display.setCursor(0,0);
  for(int Dev=0; Dev<32; ++Dev){
    if(AvailablePlayers[Dev]==true){
          display.println(Dev);
          f = true;
    }
  }
  if(!f){
    display.println("Nothing");
  }
  display.display();
*/
}
