#include "SerialParsing.h"
#include "Constants.h"

#include <errno.h>
#include <Arduino.h>
#include <stdint.h>

bool SetInvalidServer(uint8_t device_id, MessageType& msgt,uint64_t& timing){
    msgt = INVALID;
    device_id = 0;
    timing = 0;
    return false;
}

bool SetInvalidClient(uint8_t device_id, MessageType& msgt){
    msgt = INVALID;
    device_id = 0;
    return false;
}

void ReceiveChar(char* buffer, uint8_t& buffer_index, bool& msg_start,bool& msg_end){
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

bool ParseMsgServer(char* buffer, uint8_t& device_id, MessageType& MSGT, uint64_t& timing){
// Assumes message has one of two forms:
// !%d#%d#%d@ where the first %d is sender_id, second %d is device ID and   third %d is the message type 
// !%d#%d#%d#%llu@ where the first %d is sender_id, second %d is device ID, and  third %d is the message type and %llu is a time

// Check Sender ID
  if(debug){
    Serial.printf("\nClient");
    Serial.println(buffer);
    Serial.printf("\n");
  }
  char* endIndex = strtok(buffer,delimiter);
  if(endIndex==NULL){
    return SetInvalidServer(device_id,MSGT,timing);
  }
  int sender_id = atoi(endIndex);
  if(sender_id!=CLIENT){
    return SetInvalidServer(device_id,MSGT,timing);
  }
// Check Device ID
  endIndex = strtok(NULL,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    return SetInvalidServer(device_id,MSGT,timing);
  }
  device_id = atoi(endIndex);
  if(device_id<1 || device_id>MAX_DEVICES){
    return SetInvalidServer(device_id,MSGT,timing);
  }
// Parse message type. For ALIVE and TIMING, check for additional timing info
  endIndex = strtok(NULL,delimiter);
// invalid message rest of message irrelevant
  if(endIndex==NULL){
    return SetInvalidServer(device_id,MSGT,timing);
  }
  int temp = atoi(endIndex); 
// invalid message type
  if(temp<ALIVE || temp>RESET){
    return SetInvalidServer(device_id,MSGT,timing);
  }
  else{
    switch(temp){
      case ALIVE:
        MSGT = ALIVE;
        // Grab next part
        endIndex = strtok(NULL, delimiter);
        if(endIndex==NULL){
          return SetInvalidServer(device_id,MSGT,timing);
        }
        timing = strtoul(endIndex,NULL,10);
        if(errno){
          return SetInvalidServer(device_id,MSGT,timing);
        }
        break;
      case TIMING:
        MSGT = TIMING;
        // Grab next part
        endIndex = strtok(NULL, delimiter);
        if(endIndex==NULL){
          return SetInvalidServer(device_id,MSGT,timing);
        }
        timing = strtoul(endIndex,NULL,10);
        if(errno){
          return SetInvalidServer(device_id,MSGT,timing);
        }
        break;
      case LOCK_IN:
          MSGT = LOCK_IN;
          break;
      case RESET:
          MSGT = RESET;
          break;
      case INVALID:
     default:
        return SetInvalidServer(device_id,MSGT,timing);
   }
  }
  return true;
}

bool ParseMsgClient(char* buffer, uint8_t& device_id, MessageType& MSGT){
// Assumes message has the following form:
// !%d#%d#%d@ where the first %d is either SERVER or CLIENT, the second %d is the device_id and third %d is message type
  if(debug){
    Serial.printf("\nServer");
    Serial.println(buffer);
    Serial.printf("\n");
  }
// Check if first character is from SERVER
  char* endIndex = strtok(buffer,delimiter);
// If we read nothing, return invalid message
  if(endIndex==NULL){
    return SetInvalidClient(device_id,MSGT);
  }
  int sender_id = atoi(endIndex);
  if(sender_id!=SERVER){
    return false;
  }

// Check device ID
  endIndex = strtok(NULL,delimiter);
// If we read nothing, return invalid message
  if(endIndex==NULL){
    return SetInvalidClient(device_id,MSGT);
  }
// check if device_id is valid
  device_id = atoi(endIndex);
  if(device_id<1 || device_id > MAX_DEVICES){
    return SetInvalidClient(device_id,MSGT);
  }

// Grab next part
  endIndex = strtok(NULL, delimiter);
  if(endIndex==NULL){
    return SetInvalidClient(device_id,MSGT);
  }

// This should be the MessageType. Since the enum is contiguous, we can check if atoi() returns something below or above valid range of enum
  int temp = atoi(endIndex);
// invalid message type
  if(temp<ALIVE || temp>RESET){
    return SetInvalidClient(device_id,MSGT);
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
        break;
      case INVALID:
      default:
        return SetInvalidClient(device_id,MSGT);
    }
  }
  return true;
}

void SendMsgServer(uint8_t Device_ID, MessageType msgt){
// turn on transmission
  if(debug){
    Serial.printf("Send:%d,%d\n",Device_ID,msgt);
  }
    digitalWrite(ENABLE_PIN,HIGH);
    Serial.printf("!%d#%d#%d@",SERVER,Device_ID,msgt);
// Make sure data is written to bus. Currently is blocking, so can't reset in while server is sending. Shouldn't be a problem though?
  Serial.flush();
// turn on receiving
    digitalWrite(ENABLE_PIN,LOW);
}

void SendMsgClient(uint8_t Device_ID, MessageType msgt, uint64_t timing){
// turn on transmission
  if(debug){
    Serial.printf("Send:%d,%d,%llu\n",Device_ID,msgt,timing);
  }
  digitalWrite(ENABLE_PIN,HIGH);
// print formatted message
  switch(msgt){
    case TIMING:
// The client sends out the time they buzzed in. Not buzzing in corresponds to a time of 0
      Serial.printf("!%d#%d#%d#%llu@",CLIENT,Device_ID,msgt, timing);
      break;
    case ALIVE:
// The client sends out the current time since the client started up
      Serial.printf("!%d#%d#%d#%lu@",CLIENT,Device_ID,msgt, micros());
      break;
    case LOCK_IN:
    case RESET:
      Serial.printf("!%d#%d#%d@",CLIENT,Device_ID,msgt);
      break;
// Don't send a response if invalid
    case INVALID:
    default:
// send back generic response stating message type and device ID
      break;
  }
// Make sure data is written to bus. Currently blocking, so can't buzz in while client is sending. Shouldn't be a problem though, since human reaction time is ms, while this should take microseconds
  Serial.flush();
// turn on receiving
  digitalWrite(ENABLE_PIN,LOW);
}
