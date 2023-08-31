// Modifed from RandomNerdTutorials.com. See below
/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-async-web-server-espasyncwebserver-library/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "Constants.h"
#include "Wifi.h"

// Stand in for actual data
const int devices = 6;
uint8_t Rankings[devices] = {3,4,2,0,0,0};
bool DevicesAlive[devices+1] {0,0,1,1,1,0,0};

// Used to periodically check if we need to refresh client
uint64_t refresh_client_start = micros();
uint64_t refresh_client_end = refresh_client_start;

// Replace with your network credentials
const char* ssid = "test";
const char* password = "";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

String html;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup(){
  Serial.begin(115200);
  LittleFS.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Connect to Wi-Fi
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, NULL,1,0,1);

  // Print ESP Local IP Address
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(WiFi.softAPIP());
  display.display();

  initWebSocket(server, ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS,"/game_index.html","text/html");});
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS,"/index.css","text/css");});
  server.on("/Requests.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS,"/Requests.js","text/javascript");});
//server.serveStatic("/A41Sec.mp3", SPIFFS, "/A41Sec.mp3");
// Start server
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  if(refresh_client_end-refresh_client_start < refresh_client_pool){
    refresh_client_end = micros();
  }
  else{
    ws.cleanupClients();
    refresh_client_start = micros();
    refresh_client_end = refresh_client_start;
  }
  String response = RespondToWebInterface(Rankings,DevicesAlive,devices,current_webpage_update);
  if(response!=""){
    notifyClients(response, ws);
  }
}