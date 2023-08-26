#include "SerialParsing.h"
#include <errno.h>
#include <Arduino.h>
#include "Constants.h"
#include <stdint.h>

void RecieveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end){
// read in a single character from the bus if available
  char rc;
  while(Serial.available() > 0 ){
    rc = Serial.read();
//    Serial.printf("%c",rc);
    if(!msg_start){
          if(rc==start_marker){
//            Serial.println("start");
            // Point to beginning of array, and allow future reads of Serial to be processed
            buffer_index = 0;
            msg_start = true;
          }
    }
    else{
      if(rc!=end_marker){
// read in characters, then advance index
//            Serial.println("mid");
        buffer[buffer_index] = rc;
        buffer_index++;
// If the index exceeds the buffer size,t then we have gone on for too long. set msg_start and msg_end to false, reset buffer to start and return
        if(buffer_index>=bufferSize){
          msg_start = false;
          msg_end = false;
          buffer_index = 0;
          return;
        }
      }
      else{
// We are at the end of the message. Need to null terminate string
//            Serial.println("end");
        buffer[buffer_index] = '\0';
        msg_end = true;
// exit to start parsing message
        return ;
      }
    }
  }
}

void ParseMsgServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing){
// Assumes message has one of two forms:
// !%d#%d@ where the first %d is the message type and second %d is device ID
// !%d#%d#%llu@ where first %d is the message type equal to TIMING, second %d is device ID, %, and %llu is an uint64_t
  char* endIndex = strtok(buffer,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }

// First variable should be the MessageType. Since the enum is contiguous, we can check if atoi() returns something below or above valid range of enum
  int temp = atoi(endIndex); 
// invalid message type
  if(temp<ALIVE || temp>RESET){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }
  else{
    switch(temp){
      case ALIVE:
        MSGT = ALIVE;
        break;
      case RESET:
        MSGT = RESET;
        break;
      case TIMING:
        MSGT = TIMING;
        break;
      case INVALID:
      default:
        MSGT = INVALID;
        device_id = 0;
        return ;
    }
  }

// Grab next part
  endIndex = strtok(NULL, delimiter);
  if(endIndex==NULL){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }

// Switch on what MSGT is. 
  switch(MSGT){
    case ALIVE:
    case RESET:
      device_id = atoi(endIndex);
      if(device_id<1 || device_id > MAX_DEVICES){
        return;
      }
      break;
    case TIMING:
      endIndex = strtok(NULL, delimiter);
      if(endIndex==NULL){
        timing = 0;
        return ;
      }
      timing = strtoull(endIndex,NULL,10);
      if(errno==ERANGE){
        return ;
      }
      break;
// Invalid and default cascade into each other
    case INVALID:
    default:
      break;
  }
  return ;
}

void ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT){
// Assumes message has the following form
// !%d#%d@ where the first %d is the device_ID and second %d is MessageType
  char* endIndex = strtok(buffer,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }
// check if device_id is valid
  device_id = atoi(endIndex);
  if(device_id<1 || device_id > MAX_DEVICES){
    MSGT = INVALID;
    device_id = 0;
    return;
  }
// Grab next part
  endIndex = strtok(NULL, delimiter);
  if(endIndex==NULL){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }

// This should be the MessageType. Since the enum is contiguous, we can check if atoi() returns something below or above valid range of enum
  int temp = atoi(endIndex);
// invalid message type
  if(temp<ALIVE || temp>RESET){
    MSGT = INVALID;
    device_id = 0;
    return ;
  }
  else{
    switch(temp){
      case ALIVE:
        MSGT = ALIVE;
        break;
      case RESET:
        MSGT = RESET;
        break;
      case TIMING:
        MSGT = TIMING;
        break;
      case INVALID:
      default:
        MSGT = INVALID;
        device_id = 0;
        return ;
    }
  }
  return ;
}

void SendMsgServer(uint8_t Device_ID, MessageType msgt){
// turn on transmission
    digitalWrite(ENABLE_PIN,HIGH);
// print formatted message
    Serial.printf("!%d#%d@",Device_ID,msgt);
// Debugging Message. Use to send Server it's own message by shorting RX and TX pins on breadboard
//    Serial.printf("!%d#%d@",msgt,Device_ID);
// Make sure data is written to bus
  Serial.flush();
//    while(Serial.availableForWrite() > 0);
// turn on receiving
    digitalWrite(ENABLE_PIN,LOW);
}

void SendMsgClient(uint8_t Device_ID, MessageType msgt,uint64_t delta){
// turn on transmission
  digitalWrite(ENABLE_PIN,HIGH);
// print formatted message
  switch(msgt){
    case TIMING:
      Serial.printf("!%d#%d#%llu@",msgt,Device_ID,delta);
      break;
    case ALIVE:
    case RESET:
    case INVALID:
    default:
    Serial.printf("!%d#%d@",msgt,Device_ID);

  }
// Make sure data is written to bus
  Serial.flush();
//  while(Serial.availableForWrite() > 0);
// turn on receiving
  digitalWrite(ENABLE_PIN,LOW);
}

bool ServerAction(MessageType rec_msg, MessageType exp_msg, uint8_t rec_device_id, uint8_t exp_device_id, bool* DevicesAlive, uint64_t timing){
  if(rec_msg!=exp_msg && rec_device_id!= exp_device_id){
    switch(exp_msg){
      case ALIVE:
        DevicesAlive[exp_device_id] = false;
        break;
      case TIMING:
      case RESET:
      default:
        break;
    }
    return false;
  }
  switch(rec_msg){
    case ALIVE:
      DevicesAlive[rec_device_id] = true;
      return true;
      break;
    case TIMING:
      break;
    case RESET:
      break;
    case INVALID:
    default:
      break;
  }
  return false;
}

bool ClientAction(MessageType rec_msg, uint8_t rec_device_id, uint8_t exp_device_id){
  if( rec_device_id!= exp_device_id){
    return false;
  }
  switch(rec_msg){
    case ALIVE:
      SendMsgClient(rec_device_id,rec_msg);
      return true;
      break;
    case TIMING:
      break;
    case RESET:
      break;
    case INVALID:
    default:
      break;
  }
  return false;
}