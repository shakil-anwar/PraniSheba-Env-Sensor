#include "LinkedList.h"


Node_t *createNode();
Node_t *addNode(Node_t *head, Task *taskPtr);



Node_t *createNode()
{
  Node_t *temp = (Node_t*)malloc(sizeof(Node_t));
  temp -> next = NULL; //make next point to NULL
  return temp;
}
Node_t *addNode(Node_t *head, Task *taskPtr)
{
  Node_t *nodePtr;
  Node_t *temp = createNode();
  temp -> task = taskPtr;
  if (head == NULL)
  {
    //    Serial.println(F("it's head"));
    head = temp; //while linked list is empty
  }
  else
  {
    nodePtr = head;
    //    Serial.println(F("Not head"));
    //    Node_t *nodePtr;
    while (nodePtr -> next != NULL)
    {
      nodePtr = nodePtr -> next; //traverse the list until last node
    }
    nodePtr ->next = temp;  //previous last node next = current node pointer
  }

  return head;
}

//Node_t *getNode(byte nodePos)
//{
//  Node_t *ptr = taskHead;
//  for (byte i = 0; i < nodePos; i++)
//  {
//    ptr = ptr -> next;
//  }
//  return ptr;
//}

byte getSize(Node_t *head)
{
  Node_t *p = head;
  byte counter = 0;
  while (p != NULL)
  {
    counter++;
    p = p -> next;
  }
  return counter;
}
