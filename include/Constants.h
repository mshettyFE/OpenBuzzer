#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

const bool debug = false;
const bool v_debug = false;

const int SCREEN_WIDTH  = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT =  64; // OLED display height, in pixels

// Possible Messages that can be sent. 
// ALIVE is used to register a device.
// TIMING is used to get current buzz-in time of device
// LOCK_IN is used to set a client as buzzed in
// RESET is used to clear state of devices

enum MessageType {INVALID=0,ALIVE=1,TIMING=2,LOCK_IN=3, RESET=4};

enum SenderType {SERVER=1, CLIENT=2};

const int INVALID_DEVICE = 0;

// ENABLE_PIN to toggle between transmit and receive
const int ENABLE_PIN = 2;

// TRIGGER_PIN to state if button has been pushed
const int TRIGGER_PIN = 14;

// Maximum number of devices that can be connected
const int MAX_DEVICES = 5;

// Testing response wait time in microseconds.
const uint64_t WAIT_TIME = 100000;
// Actual response time. Seems to work out...
//const uint64_t WAIT_TIME = 50000;

// Start and end characters for messages
const char start_marker = '!';
const char end_marker = '@';
// Delimiter to seperate different values within a message
const char delimiter[2] = "#";

// size of buffer for both client and server (messages shouldn't exceed 32 bits)
const uint8_t bufferSize = 32;

typedef struct Record {
	uint8_t	device_id;
	uint64_t	timing;
} Rec;

#endif