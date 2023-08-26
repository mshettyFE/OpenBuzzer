#ifndef SERIAL_PARSER
#define SERIAL_PARSER
#include "Constants.h"
void RecieveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end);
void ParseMsgServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing);
void ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT);
void SendMsgServer(uint8_t Device_ID, MessageType msgt);
void SendMsgClient(uint8_t Device_ID, MessageType msgt,uint64_t delta=0);
bool ServerAction(MessageType rec_msg, MessageType exp_msg, uint8_t rec_device_id, uint8_t exp_device_id, bool* DevicesAlive, uint64_t timing=0);
bool ClientAction(MessageType rec_msg, uint8_t rec_device_id, uint8_t exp_device_id);

#endif