#include "PriorityQueue.h"
#include "Constants.h"
#include <stdio.h>

int main(){
// Set up Priority Queue with 4 possible alive clients
    PriorityQueue pq = PriorityQueue(4);
    if(pq.GetDevAlive() != 4){
        printf("Device Alive var is not set correctly\n"); 
        return 1;       
    }
    bool check;
// Insert some records into data
    Rec item1 = {1,1000};
    pq.Insert(item1);
    Rec item2 = {2,500};
    pq.Insert(item2);
    Rec item3 = {3,2000};
    pq.Insert(item3);
// Try to insert empty record into Queue. Should return false
    Rec empty = {0,0};
    check = pq.Insert(empty);
    if(check != false){
        printf("Was able to insert empty record into queue.\n");
        return 1;
    }
    Rec item4 = {4,1};
    pq.Insert(item4);
// This should fail since we are trying to insert more devices than are currently alive
    Rec item5 = {3,2000};
    check = pq.Insert(item3);
    if(check!=false){
        printf("Was able to insert more records than devices alive\n");
        return 1;
    }
    if(pq.GetLength()!=4){
        printf("Length var is not set correctly after Inserts\n"); 
        return 1;
    }
    Rec Min = pq.Pop();
    if(Min.device_id!=4){
        printf("Pop() doesn't work\n");
        return 1;
    }
    if(pq.GetLength()!=3){
        printf("Length var is not set correctly after Pop()\n"); 
        return 1;
    }
    Rec Second = pq.Pop();
    if(Second.device_id!=2){
        printf("Pop() doesn't restore binary heap property\n");
        return 1;
    }
// SetDevAlive does what it says, in addition to clearing out the queue. Hence, we expect DevAlive=3 and Length = 0
// This implicitly also tests Clear()
    pq.SetDevAlive(3);
    if(pq.GetLength()!=0 && pq.GetDevAlive()!=3){
        printf("Length var is not set correctly after Pop()\n"); 
        return 1;
    }
    printf("All good\n");
    return 0;
}