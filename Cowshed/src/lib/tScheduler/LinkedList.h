#ifndef _LINKEDLIST_
#define _LINKEDLIST_
#include <Arduino.h>

class Scheduler;
class Task;

typedef struct Node_t
{
  Task *task;
  Node_t *next;
};

Node_t *createNode();
Node_t *addNode(Node_t *head, Task *taskPtr);
//Node_t *getNode(byte nodePos);
byte getSize(Node_t *head);

#endif

