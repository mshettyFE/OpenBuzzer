#include "Constants.h"
#include "Wifi.h"
#include <ArduinoJson.h>

WebpageMessageTypes current_webpage_update = WEB_NOTHING;

String RespondToWebInterfaceTest(uint8_t* Rankings, bool* DevAlive, uint8_t max_devices, WebpageMessageTypes& msg){
// Allocate enough space to serialize everything, plus some extra just in case
    uint64_t size = 48+32*(max_devices+1)+512;
    DynamicJsonDocument  doc(size);
// cases here
    switch(msg){
      case WEB_CLEAR:
        doc["MSG"] = msg;
// clear out Rankings
        doc["Rank"][0] = 0;
        break;
      case WEB_RESCAN:
        doc["MSG"] = msg;
// Add DevicesAlive to serialization
        for(int i=0; i< max_devices; ++i){
          doc["Alive"][i] = DevAlive[i];
        }
        doc["Alive"][max_devices] = DevAlive[max_devices];
      case WEB_UPDATE:
        doc["MSG"] = msg;
// Assign message type and add Rankings to serialization
        for(int i=0; i< max_devices; ++i){
          doc["Rank"][i] = Rankings[i];
        }
        break;
      case WEB_NOTHING:
      case WEB_INVALID:
      default:
// Return Empty and reset to default
        msg = WEB_NOTHING;
        return "";
        break;
    }
    String output = "";
    serializeJson(doc,output);
    msg = WEB_NOTHING;
    return output;
}

String RespondToWebInterface(uint8_t* Rankings, bool* DevAlive, uint8_t max_devices, WebpageMessageTypes& msg){
// Allocate enough space to serialize everything, plus some extra just in case
    uint64_t size = 48+32*(max_devices+1)+512;
    DynamicJsonDocument  doc(size);
// cases here
    switch(msg){
      case WEB_CLEAR:
        doc["MSG"] = msg;
// clear out Rankings
        doc["Rank"][0] = 0;
        break;
      case WEB_RESCAN:
        doc["MSG"] = msg;
// Add DevicesAlive to serialization
        for(int i=0; i< max_devices; ++i){
          doc["Alive"][i] = DevAlive[i];
        }
        doc["Alive"][max_devices] = DevAlive[max_devices];
      case WEB_UPDATE:
        doc["MSG"] = msg;
// Assign message type and add Rankings to serialization
        for(int i=0; i< max_devices; ++i){
          doc["Rank"][i] = Rankings[i];
        }
        break;
      case WEB_NOTHING:
      case WEB_INVALID:
      default:
// Return Empty and reset to default
        msg = WEB_NOTHING;
        return "";
        break;
    }
    String output = "";
    serializeJson(doc,output);
    msg = WEB_NOTHING;
    return output;
}

void initWebSocket(AsyncWebServer& server, AsyncWebSocket& ws) {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void notifyClients(String& output_data, AsyncWebSocket& ws){
  if(output_data!=""){
    ws.textAll(output_data);
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if(debug){
        Serial.println((char*) data);
    }
    if(strcmp((char*)data, "2") == 0) {current_webpage_update = WEB_CLEAR;}
    else if(strcmp((char*)data, "3") == 0) {current_webpage_update = WEB_RESCAN;}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        if(debug){
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
          current_webpage_update = WEB_RESCAN;
        }
        break;
      case WS_EVT_DISCONNECT:
        if(debug){
          Serial.printf("WebSocket client #%u disconnected\n", client->id());
        }
        break;
      case WS_EVT_DATA:
        if(debug){
          Serial.printf("WebSocket client #%u send data\n", client->id());
        }
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

