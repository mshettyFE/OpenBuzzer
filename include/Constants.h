#ifndef CONSTANTS
#define CONSTANTS

#include <stdint.h>

const int SCREEN_WIDTH  = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT =  64; // OLED display height, in pixels

// Possible Messsages that can be sent. ALIVE is used to register a device. TIMING is used to get current buzzin time of device, and RESET is used to clear state of devices
// Server only sends out messages of the form !%d#%d@, where first %d is device ID and second %d is message type
// Client can send messages like server, or in the case of TIMING, send and addition unsigned long long
enum MessageType {INVALID,ALIVE,TIMING,RESET};

// ENABLE_PIN to toggle between transmit and receive
const int ENABLE_PIN = 2;

// Maximum number of devices that can be connected
const int MAX_DEVICES = 5;

// Response wait time in microseconds
const uint64_t WAIT_TIME = 10000000;

// Start and end characters for messages
const char start_marker = '!';
const char end_marker = '@';
// Delimiter to seperate different values within a message
const char delimiter[2] = "#";

// size of buffer for both client and server (messages shouldn't exceed 32 bits)
const uint8_t bufferSize = 32;

#endif