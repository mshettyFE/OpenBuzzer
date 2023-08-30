#ifndef SERIAL_PARSER
#define SERIAL_PARSER
#include "Constants.h"

bool SetInvalidServer(int8_t device_id, MessageType& msgt,uint64_t& timing);
bool SetInvalidClient(int8_t device_id, MessageType& msgt);
void ReceiveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end);
bool ParseMsgServer(char* buffer, int8_t& device_id, MessageType& MSGT, uint64_t& timing);
bool ParseMsgClient(char* buffer, int8_t& device_id, MessageType& MSGT);
void SendMsgServer(int8_t Device_ID, MessageType msgt);
void SendMsgClient(int8_t Device_ID, MessageType msgt, uint64_t timing=0);
#endif