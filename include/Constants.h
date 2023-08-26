#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

const int SCREEN_WIDTH  = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT =  64; // OLED display height, in pixels

// Possible Messsages that can be sent. ALIVE is used to register a device. TIMING is used to get current buzz-in time of device, and RESET is used to clear state of devices
// Server only sends out messages of the form !%d#%d@, where first %d is device ID and second %d is message type
// Client can send messages like server, or in the case of TIMING, send and addition unsigned long long

// In addition, Server can send out any valid message type, but can only receive ALIVE and TIMING as valid messages
enum MessageType {INVALID,ALIVE,TIMING,LOCK_IN, RESET};

// ENABLE_PIN to toggle between transmit and receive
const int ENABLE_PIN = 2;

// TRIGGER_PIN to state if button has been pushed
const int TRIGGER_PIN = 14;

// Maximum number of devices that can be connected
const int MAX_DEVICES = 2;

// Testing response wait time in microseconds. Set to 1 sec to allow a large margin to receive ping back
const uint64_t WAIT_TIME = 1000000;
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