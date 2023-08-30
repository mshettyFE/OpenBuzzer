#ifndef ESP8266_WIFI
#define ESP8266_WIFI

#include "Constants.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// We default to updating Website

String RespondToWebInterface(uint8_t* Rankings, bool* DevAlive, uint8_t MAX_DEVICES, WebpageMessageTypes& msg);

void initWebSocket(AsyncWebServer& server, AsyncWebSocket& ws);

void notifyClients(String& output_data, AsyncWebSocket& ws);

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);

void notFound(AsyncWebServerRequest *request);

#endif