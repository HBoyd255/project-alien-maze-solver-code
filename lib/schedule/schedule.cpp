

#include "schedule.h"

Schedule::Schedule(voidFuncPtr functionPtr)
    : _functionPtr(functionPtr), _period(500) {}

void Schedule::execute() {
    this->_functionPtr();
    this->_lastExecutionTime = millis();
}

void Schedule::poll() {
    if (millis() < _lastExecutionTime + _period) {
        this->execute();
    }
}