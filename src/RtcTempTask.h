#pragma once

#include "Task.h"
#include <NimBLEDevice.h>

#include "MainService.h"
#include "RTClib.h"

/**
 * Required to call the start method in order to start the task
 */
class RtcTempTask : public Task {
public:
    explicit RtcTempTask(MainService* mainService, uint32_t stackSize = 4096, UBaseType_t priority = 1);

    // Finds and connects to the DS3231 RTC module
    bool begin();

    // Function containing the task logic
    void main() override;
private:
    RTC_DS3231 _rtc;
    MainService* _mainService;
    float _lastTemp = -999.0;
};
