#include "tScheduler.h"

/*****************************Linked List vars*****************/
Node_t *taskHead = NULL;
Node_t *queueHead = NULL;

Task::Task(uint32_t interval, taskCb_t cb, byte priority)
{
  _Cb = cb;
  _interval = interval;
  _priority = priority;
  _prevTime = 0;
}

void Task::setInterval(uint32_t intervalTime)
{
  _interval = intervalTime;
}

void Scheduler::begin(uint32_t (*timer)(void))
{
  if (timer == NULL)
  {
    _timer = millis;
  }
  else
  {
    _timer = timer;
  }

  _totalTasks = getSize(taskHead);
  Serial.print(F("Total Tasks: ")); Serial.println(_totalTasks);
  qPtr = (Task**)malloc(_totalTasks * sizeof(Task*));
  nullQBuf();
}

void Scheduler::nullQBuf()
{
  Task **p = qPtr;
  for (byte i = 0; i < _totalTasks; i++)
  {
    *p = NULL;
    p++;
  }
}

void Scheduler::addTask(Task *taskPtr)
{
  taskHead = addNode(taskHead, taskPtr);
}

Task **Scheduler::getTaskQueue()
{
  Node_t *ptr = taskHead;
  Task **qP = qPtr;
  Task *task = NULL;

  uint32_t currentTime =  _timer();
  do
  {
    task = ptr -> task;

    if ((currentTime - task -> _prevTime) >= task -> _interval)
    {
      // Serial.println(F("--->Task added into queue--->"));
      task -> _prevTime = currentTime;
      *qP = task;
      qP++;
    }
    else
    {
      *qP = NULL;
      if(task -> _prevTime > currentTime)
      {
        task -> _prevTime = currentTime;
      }
    }

    ptr = ptr -> next;
  } while (ptr != NULL);
  return qPtr;
}

void Scheduler::run()
{
  // Serial.print(F("Time: "));Serial.println(_timer());
  Task  **taskPtr = getTaskQueue();
  if (*taskPtr)
  {
    Task **prevTask = NULL;
    for ( byte i = 0; i < _totalTasks; i++)
    {
      prevTask = taskPtr;
      if (*taskPtr != NULL)
      {
        (*taskPtr) -> _Cb();
      }
      else
      {
        // Serial.println(F("Null Pointer"));
      }
      taskPtr++;
      *prevTask = NULL;
    }
  }
}

//  Serial.println(F("<============Execution start===========>"));
//  for( byte i = 0; i < _totalTasks; i++)
//  {
//    if(*ptr !=NULL)
//    {
//      Serial.println(F("NOT NULL"));
//    }
//  }
//  nullQBuf();



//    byte tCounter = 0;
//    do
//    {
//      if(*taskPtr)
//      {
//        (*taskPtr) -> _Cb();
//      }
//      prevTask = taskPtr;
//      taskPtr++;
//      *prevTask = NULL;
//      tCounter++;
//    }while(tCounter <= _totalTasks);

