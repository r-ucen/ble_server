#pragma once

#include <NimBLEDevice.h>

class MainService {
public:
    // Configures service specific data, creates the service and its characteristics
    void begin(NimBLEServer* server);

    // Sets the tempCharacteristic value and notifies its subscribers
    void setTemperature(const char* text);
private:
    NimBLECharacteristic* _tempChar = nullptr;
    NimBLECharacteristic* _ledChar = nullptr;
};
