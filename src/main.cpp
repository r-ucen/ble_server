#include <Arduino.h>
#include <NimBLEDevice.h>
#include <nvs_flash.h>
#include "RTClib.h"
#include "secrets.h"

// BLE SERVER
#define SERVICE_UUID "54465d33-9f9f-4344-afff-e97aa90ff551"
#define TEMP_CHARACTERISTIC_UUID "c1059e53-9b8f-42d4-ab20-255118af61d3"
#define LED_CHARACTERISTIC_UUID "63d70fe8-2172-43b6-adfa-73b476378a19"

NimBLECharacteristic *tempCharacteristic = nullptr;
NimBLECharacteristic *ledCharacteristic = nullptr;

// RTC
RTC_DS3231 rtc;

// temp
float lastTemp = -999.0;

constexpr int LED_PIN = 26;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("[NimBLE] Client connected.");
        Serial.print(pServer->getConnectedCount());
        Serial.println(" clinet(s) connected");
    }

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override {
        Serial.println("[NimBLE] Client disconnected, restarting advertising...");
        Serial.print(pServer->getConnectedCount());
        Serial.println(" clinet(s) connected");
        NimBLEDevice::startAdvertising();
    }

    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
        if (connInfo.isEncrypted()) {
            Serial.println("[NimBLE] Security Pairing Successful.");
        } else {
            Serial.println("[NimBLE] Security Pairing Failed.");
        }
    }
};

class LedCharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo){
        std::string value = pCharacteristic->getValue();

        if (!value.empty()){
            if (value == "1"){
                digitalWrite(LED_PIN, HIGH);
            } else if (value == "0"){
                digitalWrite(LED_PIN, LOW);
            }
        }
    }
};

void rtcTempTask(void *pvParameters) {
    while (1){
        if (NimBLEDevice::getServer()->getConnectedCount() > 0) {
            float rtc_temp = rtc.getTemperature();

            if (fabs(rtc_temp - lastTemp) >= 0.1f) {
                lastTemp = rtc_temp;

                String tempPayload = String(rtc_temp, 2) + " °C";
                tempCharacteristic->setValue(tempPayload.c_str());
                tempCharacteristic->notify();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void createTasks(){
    xTaskCreatePinnedToCore(
        rtcTempTask,
        "RTC_TEMP_Task",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("Starting Server...");

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1) { delay(1000); }
    }

    // Production NVS Setup (Keeps paired devices remembered across reboots)
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    NimBLEDevice::init("sbs");
    NimBLEDevice::setSecurityPasskey(SERVER_PIN);
    // tells the client to show an input to enter the server pin
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    // enable bonding, man in the middle protection and secure connection pairing
    NimBLEDevice::setSecurityAuth(true, true, true);

    // create server
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());


    // SERVICES
    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    // CHARACTERISTICS
    tempCharacteristic = pService->createCharacteristic(
        TEMP_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | 
        NIMBLE_PROPERTY::WRITE | 
        NIMBLE_PROPERTY::READ_ENC | 
        NIMBLE_PROPERTY::WRITE_ENC |
        NIMBLE_PROPERTY::READ_AUTHEN | 
        NIMBLE_PROPERTY::WRITE_AUTHEN |
        NIMBLE_PROPERTY::NOTIFY
    );
    tempCharacteristic->setValue(std::string("0.0 C"));

    ledCharacteristic = pService->createCharacteristic(
        LED_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | 
        NIMBLE_PROPERTY::WRITE | 
        NIMBLE_PROPERTY::READ_ENC | 
        NIMBLE_PROPERTY::WRITE_ENC |
        NIMBLE_PROPERTY::READ_AUTHEN | 
        NIMBLE_PROPERTY::WRITE_AUTHEN
    );

    ledCharacteristic->setCallbacks(new LedCharacteristicCallbacks());

    // start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setName("sbs");
    pAdvertising->start();

    Serial.println("Advertising started.");

    createTasks();
}

void loop() {
    vTaskDelete(NULL);
}