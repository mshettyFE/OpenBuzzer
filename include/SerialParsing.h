#ifndef SERIAL_PARSER
#define SERIAL_PARSER
#include "Constants.h"
bool SetInvalidServer(uint8_t device_id, MessageType& msgt,uint64_t& timing);
bool SetInvalidClient(uint8_t device_id, MessageType& msgt);
void ReceiveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end);
bool ParseMsgServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing);
bool ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT);
void SendMsgServer(uint8_t Device_ID, MessageType msgt, uint64_t start=0);
void SendMsgClient(uint8_t Device_ID, MessageType msgt, uint64_t timing=0);
#endif