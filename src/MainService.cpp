#include "MainService.h"

#include <Arduino.h>
#include "config.h"

namespace {
    class LedCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    public:
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            const std::string value = pCharacteristic->getValue();

            if (!value.empty()){
                if (value == "1"){
                    digitalWrite(LED_PIN, HIGH);
                } else if (value == "0"){
                    digitalWrite(LED_PIN, LOW);
                }
            }
        }
    };

    LedCharacteristicCallbacks ledCallbacks;
}

void MainService::begin(NimBLEServer *server) {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    NimBLEService *service = server->createService(MAIN_SERVICE_UUID);

    _tempChar = service->createCharacteristic(
        TEMP_CHARACTERISTIC_UUID,
        READ | READ_AUTHEN | NOTIFY
    );
    _tempChar->setValue(std::string("0.0 C"));

    _ledChar = service->createCharacteristic(
        LED_CHARACTERISTIC_UUID,
        READ | READ_AUTHEN | WRITE | WRITE_AUTHEN
    );
    _ledChar->setCallbacks(&ledCallbacks);
}

void MainService::setTemperature(const char *text) {
    _tempChar->setValue(text);

    const auto notifyResult = _tempChar->notify();
    if (!notifyResult) {
        Serial.println("Notifying subscribers to the tempCharacteristic failed.");
    }
}
