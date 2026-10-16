#include "BleServer.h"
#include <nvs_flash.h>
#include <Arduino.h>

namespace {
    class ServerCallbacks : public NimBLEServerCallbacks {
    public:
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
            Serial.println("[Server] Client connected.");
            Serial.print(pServer->getConnectedCount());
            Serial.println(" clinet(s) connected");
        }

        void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override {
            Serial.println("[Server] Client disconnected, restarting advertising...");
            Serial.print(pServer->getConnectedCount());
            Serial.println(" clinet(s) connected");
            NimBLEDevice::startAdvertising();
        }

        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
            if (connInfo.isEncrypted()) {
                Serial.println("[Server] Security Pairing Successful.");
            } else {
                Serial.println("[Server] Security Pairing Failed.");
            }
        }
    };

    ServerCallbacks serverCallbacks;
}

void BleServer::init(const char *deviceName, const uint32_t passkey) {
    Serial.println("Starting Server...");

    // preserve bonding information in NVS
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    NimBLEDevice::init(deviceName);
    NimBLEDevice::setSecurityPasskey(passkey);
    // tells the client to show an input to enter the server pin
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    // enable bonding, man in the middle protection and secure connection pairing
    NimBLEDevice::setSecurityAuth(true, true, true);

    // create server
    _server = NimBLEDevice::createServer();
    _server->setCallbacks(&serverCallbacks);
}

void BleServer::startAdvertising(const char *deviceName, std::vector<std::string> serviceUUIDS) {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

    for (const auto& service: serviceUUIDS) {
        const auto res = pAdvertising->addServiceUUID(service.c_str());
        if (!res) {
            auto msg = String("Failed adding service with UUID") + service.c_str();
            Serial.println(msg);
        }
    }
    pAdvertising->setName(deviceName);
    pAdvertising->start();

    Serial.println("Advertising started.");
}

NimBLEServer * BleServer::server() const {
    return _server;
}

size_t BleServer::connectedCount() const {
    return _server->getConnectedCount();
}
