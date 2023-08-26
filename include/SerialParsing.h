#ifndef SERIAL_PARSER
#define SERIAL_PARSER
#include "Constants.h"
void RecieveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end);
void ParseMsgServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing);
void ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT);
void SendMsgServer(uint8_t Device_ID, MessageType msgt);
void SendMsgClient(uint8_t Device_ID, MessageType msgt,uint64_t delta=0);

#endif