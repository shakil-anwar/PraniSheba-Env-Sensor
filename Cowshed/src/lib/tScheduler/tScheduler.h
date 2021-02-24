#ifndef _T_SCHEDULER_
#define _T_SCHEDULER_
#include <Arduino.h>
#include "LinkedList.h"
typedef void (*taskCb_t)(void);

class Task
{
    friend class Scheduler;
  public:
    Task(uint32_t interval, taskCb_t cb, byte priority = 0);
    void setInterval(uint32_t intervalTime);
  private:
    taskCb_t _Cb;
    uint32_t _interval;
    uint32_t _prevTime;
    byte _priority;
    

};

class Scheduler
{
    friend class Task;
  public:
    void begin(uint32_t (*timer)(void) = NULL);
    void addTask(Task *taskPtr);
    void removeTask(Task *taskPtr);
    Task **getTaskQueue();
    void run();
    void nullQBuf();
  private:
    uint32_t (*_timer)(void);
    Task **qPtr;
    byte _totalTasks = 0;
};
#endif
