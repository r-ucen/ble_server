#pragma once

#include <NimBLEDevice.h>

class BleServer {
public:

    // Creates and configures the server instance
    void init(const char* deviceName, uint32_t passkey);

    // Adds services, assigns device name and starts advertising
    void startAdvertising(const char* deviceName, const std::vector<std::string>& serviceUUIDS);

    NimBLEServer* server() const;
    size_t connectedCount() const;
private:
    NimBLEServer* _server = nullptr;
};
