#include <errno.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include "Constants.h"
#include "SerialParsing.h"
#include "PriorityQueue.h"
#include "Wifi.h"

// Network credentials
const char* ssid = "test";
const char* password = "";

// using 1 based indexing, since atoi returns 0 for no conversion. Want to avoid this ambiguity.
bool DevicesAlive[MAX_DEVICES+1];

// place to store timing data from ALIVE messages
uint64_t starting_times[MAX_DEVICES];

// Offset used to synchronize clocks amongst processors. If buzz in time greater than this offset, subtract of this offset
// If buzz in time is less than this offset, just use buzz in time
// This ensures that all clocks have a common starting point in-spite of the fact that each client's internal starting point is difference and subject to rollover
uint64_t synchronized_start_time = 0;

// Rankings of players
uint8_t RankingIndex = 0;
uint8_t Rankings[MAX_DEVICES];
// Index to keep track of if Rankings has been updated
uint8_t RankingIndexPrevious = 0;

// Priority queue to assign order of buzzing in
PriorityQueue BuzzInTimes(0);
// Place to store if a player has buzzed in or not
bool BuzzedIn[MAX_DEVICES+1];

// Buffer for incoming message;
char buffer[bufferSize];

// places to store timing data
uint64_t timing;

// Reset flag
bool reset_flag  = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Used to refresh clients that disconnect
uint64_t refresh_client_start = micros();
uint64_t refresh_client_end = refresh_client_start;

void IRAM_ATTR TogglePressed(){
 reset_flag = true; 
}

bool ServerAction(MessageType rec_msg, MessageType exp_msg, int8_t rec_device_id, int8_t exp_device_id, uint64_t timing){
  if(v_debug){
    Serial.printf("\nAction:%d,%d,%d,%d,%llu\n",rec_msg,exp_msg,rec_device_id,exp_device_id,timing);
  }
  if(rec_msg!=exp_msg && rec_device_id!= exp_device_id){
    switch(exp_msg){
      case ALIVE:
        DevicesAlive[exp_device_id] = false;
        break;
      case TIMING:
      case LOCK_IN:
      case RESET:
      case INVALID:
      default:
        break;
    }
    return false;
  }

  switch(rec_msg){
    case ALIVE:
      DevicesAlive[rec_device_id] = true;
// We assign the latest start 
      starting_times[rec_device_id] = timing;
      return true;
      break;
    case TIMING:
// If the client has buzzed in previously, return now. We can't include the same client twice
      if(BuzzedIn[rec_device_id]){
        return true;
      }
// If we got here, this means that the client has not buzzed in before. 
// We check if timing is invalid (ie. zero)
    if(timing==0){
      return true;
    }
/* There is a problem with this code. Suppose that one client connects much later that the rest of the clients.
 Most clients roll over, but the lagging client doesn't.
 When all clients buzzing, the lagging client will have a much higher time compared to the others.
 This means that the lagging client will never be able to buzz in if someone else buzzes

 The above bug can be fixed by Rescanning (ie. Sending out a new alive message) the system whenever a new client connects. This does two things:
  - Registers the new device to the DevicesALive array
  - Updates the synchronization time to include the new device.

 TL;DR There technically is a bug here, but it shouldn't matter as long as you Rescan the system whenever a new player joins
*/

// If the client is ahead of the synchronized start time, we subtract off the start time in order to keep in line with any lagging clients
      Rec rc;
      if(timing>synchronized_start_time){
        rc = {rec_device_id,timing-synchronized_start_time};
      }
// Otherwise, we read the time as is
      else{
        rc  = {rec_device_id,timing};
      }
// Try to insert device into priority queue
      if(!BuzzInTimes.Insert(rc)){
        BuzzedIn[rec_device_id] = false;
      }
      else{
        BuzzedIn[rec_device_id] = true;
      }
      break;
    case LOCK_IN:
         break;
    case RESET:
        break;
    case INVALID:
    default:
      break;
  }
  return false;
}

void ServerPostPollingActions(MessageType msg){
// Keep track of how many devices are currently alive
  int device_alive =0;
// We look at the times submitted by all devices. We set the largest value seen to be the synchronized_start_time
  uint64_t max_time =0;
  switch(msg){
    case ALIVE:
      for(int Device=0; Device<MAX_DEVICES; Device++){
        if(DevicesAlive[Device]){
// Add one to device alive
          device_alive++;
// Check if submitted time is larger than current max. Update current max as needed
          if(starting_times[Device] > max_time){
            max_time = starting_times[Device];
          }
        }
      }
// Update PriorityQ to Reflect current state
      BuzzInTimes.SetDevAlive(device_alive);
// set synchronization time to maximum time posted from clients
      if(max_time != 0){
        synchronized_start_time = max_time;
      }
      break;
    case TIMING:
// There should not be any repeats since ServerAction prevents that via BuzzedIn bool array
      while(BuzzInTimes.GetLength() != 0){
        Rec cur = BuzzInTimes.Pop();
        if(cur.device_id==INVALID_DEVICE){
          continue;
        }
// Something horrible happened if this if statement gets triggered
        if(RankingIndex>MAX_DEVICES){
          break;          
        }
        Rankings[RankingIndex] = cur.device_id;
        RankingIndex++;
      }
      break;
    case RESET:
// turn of reset flag so that we don't immediately trigger reset again
      reset_flag  = false;
// Clear Priority queue
      BuzzInTimes.Clear();
      if(debug){
        Serial.printf("\n\n\n%d\n\n\n",BuzzInTimes.GetLength());
      }
// Set last slot of BuzzedIn to false
      BuzzedIn[MAX_DEVICES] = false;
// Set RankingIndex to first item
      RankingIndex = 0;
//  Clear BuzzedIn, starting_times, and Rankings
      for(int Device=0; Device<MAX_DEVICES;++Device){
        BuzzedIn[Device] = false;
        starting_times[Device] = 0;
        Rankings[Device] = 0;
      }
      break;
// Don't do anything on these messages
    case LOCK_IN:
      break;
    case INVALID:
    default:
      break;
  }
  if(RankingIndex!=RankingIndexPrevious){
    RankingIndexPrevious = RankingIndex;
    current_webpage_update = WEB_UPDATE;
  }
}

void SendMessage(int Device, uint64_t WaitTime, MessageType msg){
  bool msg_start,msg_end;
  uint8_t buffer_index;
  int8_t received_device_id;
  uint64_t start_time,end_time;
  MessageType received_msg;
// dont send anything if Device=0
  if(Device==0){
    return;
  }
// Fire off message for device Device
  SendMsgServer(Device,msg);
// set up local timing variables
  received_msg = INVALID;
  received_device_id = 0;
  msg_start = false;
  msg_end = false;
  buffer_index = 0;
  start_time = micros();
  end_time = micros();
  bool v;
// wait for message for at most WaitTime microseconds
  while((end_time-start_time) < WaitTime){
    ReceiveChar(buffer, buffer_index, msg_start, msg_end);
    if(msg_end && msg_start){
      v = ParseMsgServer(buffer,received_device_id,received_msg,timing);
      break;
    }
    end_time = micros();
  }
  if(v){
    ServerAction(received_msg, msg, received_device_id,Device,timing);
  }
}

void ScanForDevices(uint64_t WaitTime, MessageType msg){
  for(int Device=1; Device<=MAX_DEVICES; Device++){
    if(msg==ALIVE){
      SendMessage(Device,WaitTime,msg);
    }
    else{
      if(DevicesAlive[Device]){
       SendMessage(Device,WaitTime,msg);
      } 
    }
  }
}

void SendMessageToAll(MessageType msg){
  uint64_t start_time,end_time;
// Fire off message for all devices
  SendMsgServer(ALL_DEVICES,msg);
// set up local timing variables
  start_time = micros();
  end_time = micros();
// wait for SEND_ALL_WAIT_TIME to give enough time for clients to process request
  while((end_time-start_time) < SEND_ALL_WAIT_TIME){
    end_time = micros();
  }
}

void UpdateServerDisplayDebug(){
  display.clearDisplay();
  display.setCursor(0,0);
// Print Alive candidates
  display.printf("Alive: ");
  for(int d=0; d<=MAX_DEVICES; ++d){
    display.printf("%d,",DevicesAlive[d]);
  }
  display.printf("\n");
// Print Ranking
  display.printf("Ranking: ");
  for(int i=0; i<MAX_DEVICES; ++i){
    display.printf("%d,",Rankings[i]);
  }
  display.printf("\n");
// Display Who is Buzzed in
  display.printf("Buzz: ");
  for(int i=0; i<=MAX_DEVICES; ++i){
    display.printf("%d,",BuzzedIn[i]);
  }
  display.printf("\n");
// Print 
  display.printf("\nOffset: %llu",synchronized_start_time);
  display.printf("\nLocked: %d",Rankings[0]);
  display.display();
}

void setup() {
  if(debug){
    Serial.printf("\n\n\n\n");
  }
  LittleFS.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
    // Connect to Wi-Fi
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, NULL,1,0,1);
  initWebSocket(server, ws);
  display.clearDisplay();
  display.printf("SSID: %s\n",ssid);
  display.println(WiFi.softAPIP());
  display.println("Connect to SSID. Type in IP in browser.");
  display.display();
  Serial.begin(115200);
  pinMode(ENABLE_PIN,OUTPUT);
  digitalWrite(ENABLE_PIN,HIGH);
  pinMode(TRIGGER_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), TogglePressed, FALLING);
// We set Device 0 to false. No client should have 0 as their id to avoid collision with atoi default of 0
  for(int Device=0; Device <=MAX_DEVICES; Device++){
    DevicesAlive[Device] = false;
    BuzzedIn[Device] = false;
  }
  for(int Device=0; Device<MAX_DEVICES; Device++){
    Rankings[Device] = 0;
  }
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(LittleFS,"/game_index.html","text/html");
              current_webpage_update = WEB_UPDATE;
              });
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS,"/index.css","text/css");});
  server.on("/Requests.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS,"/Requests.js","text/javascript");});
// Scan all devices to check ALIVE connections and get global offset time
// Start server
  server.onNotFound(notFound);
  server.begin();
  ScanForDevices(WAIT_TIME,ALIVE);
  ServerPostPollingActions(ALIVE);
}

void loop() {
  ScanForDevices(WAIT_TIME,TIMING);
  ServerPostPollingActions(TIMING);
// Send Message to Top Ranker
  SendMessage(Rankings[0],WAIT_TIME,LOCK_IN);
  if(reset_flag){
// Check if any new connections joined. Also to update synchronization time
// Need to remove ALIVE messages in final version. You will be able to RESCAN via the web app
// Reset buzzIn on all devices
    SendMessageToAll(RESET);
    ServerPostPollingActions(RESET);
    reset_flag = false;
  }
  if(refresh_client_end-refresh_client_start < refresh_client_pool){
    refresh_client_end = micros();
  }
  else{
    ws.cleanupClients();
    refresh_client_start = micros();
    refresh_client_end = refresh_client_start;
  }

  if(current_webpage_update!=WEB_NOTHING){
    String response;
    switch(current_webpage_update){
      case WEB_UPDATE:
        response = RespondToWebInterface(Rankings,DevicesAlive,MAX_DEVICES,current_webpage_update);
        break;
      case WEB_RESCAN:
        ScanForDevices(WAIT_TIME,ALIVE);
        ServerPostPollingActions(ALIVE);
        response = RespondToWebInterface(Rankings,DevicesAlive,MAX_DEVICES,current_webpage_update);
        break;
      case WEB_CLEAR:
        SendMessageToAll(RESET);
        ServerPostPollingActions(RESET);
        reset_flag = false;
        response = RespondToWebInterface(Rankings,DevicesAlive,MAX_DEVICES,current_webpage_update);
        break;
      case WEB_NOTHING:
      default:
        response = RespondToWebInterface(Rankings,DevicesAlive,MAX_DEVICES,current_webpage_update);
        break;
    }
    if(response!=""){
      notifyClients(response, ws);
    }
  }
  UpdateServerDisplayDebug();
}