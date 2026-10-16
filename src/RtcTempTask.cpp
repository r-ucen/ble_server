#include "RtcTempTask.h"

RtcTempTask::RtcTempTask(MainService *mainService, const uint32_t stackSize, const UBaseType_t priority)
    : Task(stackSize, priority, "RTC_TEMP_Task"), _mainService(mainService) {}

bool RtcTempTask::begin() {
    if (!_rtc.begin()) {
        Serial.println("Could not find RTC");
        return false;
    }
    return true;
}

void RtcTempTask::main() {
    for (;;) {
        if (NimBLEDevice::getServer()->getConnectedCount() > 0) {
            const float rtcTemp = _rtc.getTemperature();

            if (fabs(rtcTemp - _lastTemp) >= 0.1f) {
                _lastTemp = rtcTemp;

                String tempPayload = String(rtcTemp, 2) + " °C";
                _mainService->setTemperature(tempPayload.c_str());
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

