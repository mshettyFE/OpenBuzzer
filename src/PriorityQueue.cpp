#include "Constants.h"
#include "PriorityQueue.h"
#include <math.h>
#include <stdio.h>

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
// Fill priority queue with empty records
  for(int i=0; i< MAX_DEVICES; ++i){
    heap[i] = {0,0};
  }
}

PriorityQueue::~PriorityQueue(){}

bool PriorityQueue::SetDevAlive(uint8_t devices_alive){
  if(devices_alive>MAX_DEVICES && devices_alive < 0){
    return false;
  }
  Clear();
  dev_alive = devices_alive;
  return true;
}

void PriorityQueue::Clear(){
  for(int i=0; i< dev_alive; ++i){
    Pop();
  }
  length = 0;
}


uint8_t PriorityQueue::GetDevAlive(){
    return dev_alive;
};


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

bool  PriorityQueue::Insert(Rec record){
// Can't insert empty record
    if(record.timing==0 && record.device_id==0){
        return false;
    }
// if adding one more value will exceed the total number of devices currently alive, then exit early
    if(length+1 > dev_alive){
        return false;
    }
// Sanity check
    if(length+1 > MAX_DEVICES){
        return false;
    }
// Edit length, insert record at end of heap, then heapify up to restore min heap property
    length += 1;
    heap[length-1] = record;    
    HeapifyUp(length-1);
    return true;
}

Rec PriorityQueue::Pop(){
  Rec Output;
// Check if length  equals 0. If it does, then return empty node
  if(length==0){
    Output = {0,0};
    return Output;
  }
// If length is equal to 1, return Root Node and empty Queue
  if(length==1){
    Output = heap[0];
    heap[0] = {0,0};
    length = length -1;
    return Output;
  }
// Other cases: We set the output to root, set root to 0, move last element to root, then Heapifydown to restore binary heap property
  Output = heap[0];
  heap[0] = heap[length-1];
  heap[length-1] = {0,0};
  HeapifyDown(0);
  length = length-1;
  return Output;
}


void PriorityQueue::HeapifyUp(uint8_t current_index){
// Check if current index is valid. If not, return
    if(current_index <0 || current_index>=MAX_DEVICES){
        return;
    }
// Get parent node of current index and check if out of bounds
    int ParentIndex = Parent(current_index);
    if(ParentIndex <0 || ParentIndex>=MAX_DEVICES){
        return;
    }
// If current_index is the root node, we stop
    if(current_index==0){
        return;
    }
// Check if Parent node is empty (device_id and timing are both 0). If the node is empty, then we unconditionally swap nodes and recurse up
    Rec temp = heap[ParentIndex];
    if(temp.device_id==0 && temp.timing==0){
        heap[ParentIndex] = heap[current_index];
        heap[current_index] = temp;
        HeapifyUp(ParentIndex);
        return;
    }

// If the node is non-empty, then we only recurse up if the parent node has a larger timing than the current node
    if( temp.timing > heap[current_index].timing){
        heap[ParentIndex] = heap[current_index];
        heap[current_index] = temp;
        HeapifyUp(ParentIndex);
        return;
    }
}

void PriorityQueue::HeapifyDown(uint8_t current_index){
// Check if current index is valid. If not, return
    if(current_index <0 || current_index>=MAX_DEVICES){
        return;
    }
// Check if left and right indicies are valid
    int LeftIndex = Left(current_index);
    int RightIndex = Right(current_index);
    bool leftValid, rightValid;
    if(LeftIndex <0 || LeftIndex>=MAX_DEVICES){
        leftValid = true;
    }
    if(RightIndex <0 || RightIndex>=MAX_DEVICES){
        rightValid = true;
    }
// check if left and right index are occupied. If index is invalid ,treat as empty node
    bool LeftEmpty, RightEmpty;
    Rec LeftNode, RightNode;
    if(leftValid){
        LeftNode = heap[LeftIndex];
        if(LeftNode.device_id==0 && LeftNode.timing==0){
            LeftEmpty = true;
        }
        else{
          LeftEmpty = false;
        }
    }
    else{
        LeftEmpty = true;
    }
    if(rightValid){
        RightNode = heap[RightIndex];
        if(RightNode.device_id==0 && RightNode.timing==0){
            RightEmpty = true;
        }
        else{
          RightEmpty = false;
        }
    }
    else{
        RightEmpty = true;
    }
// 4 cases to consider:
//Both left and right are empty. This means current index is a leaf. Do nothing
    if(LeftEmpty&&RightEmpty){
        return;
    }
// Left is empty, but right is not. Check if we need to recurse on right (ie. check if right is smaller than current)
    if(LeftEmpty&&!RightEmpty){
        if(RightNode.timing < heap[current_index].timing){
            heap[RightIndex]= heap[current_index];
            heap[current_index] = RightNode;
            HeapifyDown(RightIndex);
        }
            return;
    }
// Right is empty, but left is not. Check if we need to recurse on left (ie. check if left is smaller than current)
    if(!LeftEmpty&&RightEmpty){
        if(LeftNode.timing < heap[current_index].timing){
            heap[LeftIndex]= heap[current_index];
            heap[current_index] = LeftNode;
            HeapifyDown(LeftIndex);
        }
            return;
    }
// neither nodes are empty. Compare which node is smaller, then swap if needed
    if(!LeftEmpty&&!RightEmpty){
        if(LeftNode.timing < RightNode.timing){
          if(LeftNode.timing < heap[current_index].timing){
            heap[LeftIndex]= heap[current_index];
            heap[current_index] = LeftNode;
            HeapifyDown(LeftIndex);
          }
          return;
        }
        else{
          if(RightNode.timing < heap[current_index].timing){
              heap[RightIndex]= heap[current_index];
              heap[current_index] = RightNode;
              HeapifyDown(RightIndex);
          }
          return;
        }
    }
    return;
}

void PriorityQueue::PrintHeap(){
    for(int i=0; i<MAX_DEVICES; ++i){
        printf("%d,%d,%llu\n",i,heap[i].device_id,heap[i].timing);
    }
}
