#include <Arduino.h>

#include "BleServer.h"
#include "config.h"
#include "MainService.h"
#include "RtcTempTask.h"
#include "secrets.h"

BleServer server;
MainService mainService;

RtcTempTask rtcTempTask(&mainService);

void setup() {
    Serial.begin(115200);

    rtcTempTask.begin();

    server.init(DEVICE_NAME, SERVER_PIN);

    mainService.begin(server.server());
    server.startAdvertising(DEVICE_NAME, {MAIN_SERVICE_UUID});

    rtcTempTask.start();
}

void loop() {
    vTaskDelete(nullptr);
}