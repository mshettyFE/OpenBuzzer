#include "SerialParsing.h"
#include "Constants.h"

#include <errno.h>
#include <Arduino.h>
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
// !%d#%d#%llu@ where first %d is the message type, second %d is device ID, %, and %llu is a time

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
      case TIMING:
        MSGT = TIMING;
        break;
      case LOCK_IN:
          MSGT = LOCK_IN;
          break;
      case RESET:
          MSGT = RESET;
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
// Timing  and Alive both expect an unsigned long as well
// Alive expects a time to properly set the offset
// Timing expects a time to properly determined who buzzed in
    case ALIVE:
    case TIMING:
      device_id = atoi(endIndex);
      if(device_id<1 || device_id > MAX_DEVICES){
        return;
      }
      endIndex = strtok(NULL, delimiter);
      if(endIndex==NULL){
        timing = 0;
        return ;
      }
      timing = strtoull(endIndex,NULL,10);
      if(errno==ERANGE){
        MSGT = INVALID;
        device_id = 0;
        return ;
      }
      break;
// Cascade these together since all of these messages are good
    case LOCK_IN:
    case RESET:
    case INVALID:
    default:
      break;
  }
  return ;
}

void ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT){
// Assumes message has the following form:
// !%d#%d@ where the first %d is the device_id and second %d is message type
// NOTE: This is the opposite of what the server expects (message type and device ID)

  char* endIndex = strtok(buffer,delimiter);
// If we read nothing, return invalid message
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
      case LOCK_IN:
        MSGT = LOCK_IN;
      case INVALID:
      default:
        MSGT = INVALID;
        device_id = 0;
        return ;
    }
  }
  return ;
}

void SendMsgServer(uint8_t Device_ID, MessageType msgt, uint64_t start){
// turn on transmission
    digitalWrite(ENABLE_PIN,HIGH);
    switch(msgt){
      case ALIVE:
      case TIMING:
      case LOCK_IN:
      case RESET:
        Serial.printf("!%d#%d@",Device_ID,msgt);
        break;
      case INVALID:
      default:
      break;
    }
// Make sure data is written to bus. Currently is blocking, so can't reset in while server is sending. Shouldn't be a problem though?
  Serial.flush();
// turn on receiving
    digitalWrite(ENABLE_PIN,LOW);
}

void SendMsgClient(uint8_t Device_ID, MessageType msgt, uint64_t timing){
// turn on transmission
  digitalWrite(ENABLE_PIN,HIGH);
// print formatted message
  switch(msgt){
    case TIMING:
// The client sends out the time they buzzed in. Not buzzing in corresponds to a time of 0
      Serial.printf("!%d#%d#%llu@",msgt,Device_ID, timing);
      break;
    case ALIVE:
// The client sends out the current time since the client started up
      Serial.printf("!%d#%d#%lu@",msgt,Device_ID, micros());
      break;
    case LOCK_IN:
    case RESET:
    case INVALID:
    default:
      Serial.printf("!%d#%d@",msgt,Device_ID);
// send back generic response stating message type and device ID
      break;
  }
// Make sure data is written to bus. Currently blocking, so can't buzz in while client is sending. Shouldn't be a problem though, since human reaction time is ms, while this should take microseconds
  Serial.flush();
// turn on receiving
  digitalWrite(ENABLE_PIN,LOW);
}
