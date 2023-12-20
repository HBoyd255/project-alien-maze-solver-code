#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <Arduino.h>

class Schedule {
   public:
    Schedule(voidFuncPtr functionPtr);

    void execute();
    void poll();

   private:
    voidFuncPtr _functionPtr;
    uint32_t _period = 0;
    uint32_t _lastExecutionTime = 0;
};

#endif  // SCHEDULE_H
