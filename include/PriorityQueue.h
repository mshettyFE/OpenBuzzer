#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include "Constants.h"

class PriorityQueue {
// define array of Records
// Root is at index 0
private:
// Actual heap is instantiated large enough to hold the maximum possible number of devices
    Rec heap[MAX_DEVICES];
// dev_alive indicates how the maximum devices allows in the priority queue (say MAX_DEVICES is 10, but only 3 clients are connected. dev_alive=3 since we don't need the full heap)
    uint8_t dev_alive;
    uint8_t length;
public:
      PriorityQueue(uint8_t dev_alive);
      ~PriorityQueue();
    bool Insert(Rec record);
    uint8_t Pop();
    void Clear();
        uint8_t GetLength();
private:
      int Parent(uint8_t index);
      int Left(uint8_t index);
      int Right(uint8_t index);
      bool ShiftUp(uint8_t index);
      bool ShiftDownLeft(uint8_t index);
      bool ShiftDownRight(uint8_t index);
};
  
#endif