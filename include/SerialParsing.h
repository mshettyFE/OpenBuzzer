#ifndef SERIAL_PARSER
#define SERIAL_PARSER
#include "Constants.h"
void RecieveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end);
void ParseCharacterServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing);
//  bool ParseMsgClient();
  void SendMsgServer(int Device_ID, MessageType msgt);
//  void SendMsgClient();
#endif