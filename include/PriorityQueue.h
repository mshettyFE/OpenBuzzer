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
      Rec Pop();
      void Clear();
      bool SetDevAlive(uint8_t devices_alive);
      uint8_t GetDevAlive();
      uint8_t GetLength();
private:
// Get Parent of Node
      int Parent(uint8_t index);
// Get Left and right children of node
      int Left(uint8_t index);
      int Right(uint8_t index);
// Reestablish heap property by moving up heap. Used in Insert
      void HeapifyUp(uint8_t current_index);
// Reestablish heap property by moving downwards. Used in Pop
      void HeapifyDown(uint8_t current_index);
};
  
#endif