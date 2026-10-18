#pragma once

#include <Arduino.h>


class Task {
public:
    virtual ~Task() = default;
    Task(uint32_t stackSize, UBaseType_t priority, const char* taskName = "");

    // Start and launch the task
    void start();

    // Supposed to be overridden in the base class with the task's logic
    virtual void main() = 0;

    static void wrapper(void *pvParameters);

protected:
    TaskHandle_t taskHandle;
    uint32_t _stackSize;
    UBaseType_t _priority;
    const char* _taskName;
};