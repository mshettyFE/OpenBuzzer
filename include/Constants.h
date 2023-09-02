#ifndef CONSTANTS
#define CONSTANTS

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <stdint.h>

const bool debug = false;
const bool v_debug = false;

const uint8_t SCREEN_WIDTH  = 128; // OLED display width, in pixels
const uint8_t SCREEN_HEIGHT =  64; // OLED display height, in pixels

// Possible actions to update web interface. WEB_UPDATE only send out by server
// WEB_CLEAR, WEB_RESCAN initiated by client

enum WebpageMessageTypes {WEB_INVALID,WEB_UPDATE,WEB_CLEAR,WEB_RESCAN,WEB_NOTHING};

const IPAddress local_IP(192,168,4,22);
const IPAddress gateway(192,168,4,9);
const IPAddress subnet(255,255,255,0);

// Possible Messages that can be sent. 
// ALIVE is used to register a device.
// TIMING is used to get current buzz-in time of device
// LOCK_IN is used to set a client as buzzed in
// RESET is used to clear state of devices

enum MessageType {INVALID=0,ALIVE=1,TIMING=2,LOCK_IN=3, RESET=4};

enum SenderType {SERVER=1, CLIENT=2};

const int8_t INVALID_DEVICE = 0;
const int8_t ALL_DEVICES = -1;

// ENABLE_PIN to toggle between transmit and receive
const uint8_t ENABLE_PIN = 2;

// TRIGGER_PIN to state if button has been pushed
const uint8_t TRIGGER_PIN = 14;

// I2S DAC pins
const uint8_t LRCLK_PIN= 12;
const uint8_t BCLK_PIN= 13;
const uint8_t DIN_PIN= 0;

// Maximum number of devices that can be connected
const uint8_t MAX_DEVICES = 10;

// Testing response wait time in microseconds.
const uint64_t WAIT_TIME = 100000;
// SendAll wait time in microseconds
const uint64_t SEND_ALL_WAIT_TIME = 500000;
// Webserver client refresh time
const uint64_t refresh_client_pool = 1000000;

// Start and end characters for messages
const char start_marker = '!';
const char end_marker = '@';
// Delimiter to seperate different values within a message
const char delimiter[2] = "#";

// size of buffer for both client and server (messages shouldn't exceed 32 bits)
const uint8_t bufferSize = 32;

typedef struct Record {
	int8_t	device_id;
	uint64_t	timing;
} Rec;

extern WebpageMessageTypes current_webpage_update;

#endif