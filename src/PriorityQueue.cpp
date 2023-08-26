#include "Constants.h"
#include "PriorityQueue.h"
#include <math.h>

PriorityQueue::PriorityQueue(uint8_t devices_alive){
// priority queue is empty
    length = 0;
  if(devices_alive>MAX_DEVICES){
// THis shouldn't happen, since devices_alive will come from Scanning all devices up to MAX_DEVICES
    dev_alive=0;
  }
  else{
    dev_alive = devices_alive;
  }
}

PriorityQueue::~PriorityQueue(){}

uint8_t PriorityQueue::GetLength(){
    return length;
}


int PriorityQueue::Parent(uint8_t index){
// calculates parent of node. If node is root, parent is -1 (a nonsensical value for an array on C++)
  return floor((index-1)/2.0);
}

int PriorityQueue::Left(uint8_t index){
    return 2*index+1;
}

int PriorityQueue::Right(uint8_t index){
    return 2*index+2;
}

bool PriorityQueue::ShiftUp(uint8_t index){
// Swaps node with it's parent unconditionally. Will destroy the binary heap structure if not properly managed
// can't move elements if they don't exists (need to make sure that Insert properly updates length)
    if(length==0){
        return false;
    }
    int ParentIndex = Parent(index);
// We are trying to swap the root up one. Invalid operation
// We also check that we aren't out of bounds of the heap
    if(ParentIndex==-1 || ParentIndex >= length){
        return false;
    }
    Rec temp = heap[index];
    heap[index] = heap[ParentIndex];
    heap[ParentIndex] =  temp;
    return true;
}

bool PriorityQueue::ShiftDownLeft(uint8_t index){
    if(length==0){
        return false;
    }
    int LeftIndex = Left(index);
// The maximum allowed index is length-1. Hence, if the projected index is larger than this, we can't shift down
// Once again, this depends on Insert properly adjusting length
    if(LeftIndex==-1 || LeftIndex >= length){
        return false;
    }
    Rec temp = heap[index];
    heap[index] = heap[LeftIndex];
    heap[LeftIndex] =  temp;
    return true;
}

bool PriorityQueue::ShiftDownRight(uint8_t index){
    if(length==0){
        return false;
    }
    int RightIndex = Right(index);
// The maximum allowed index is length-1. Hence, if the projected index is larger than this, we can't shift down
// Once again, this depends on Insert() properly adjusting length
    if(RightIndex==-1 || RightIndex >= length){
        return false;
    }
    Rec temp = heap[index];
    heap[index] = heap[RightIndex];
    heap[RightIndex] =  temp;
    return true;
}
