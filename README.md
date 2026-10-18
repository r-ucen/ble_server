This project resembles a Bluetooth Low Energy server with authentication using the NumBLE library.

It features an example service that has a characteristic that notifies the client with temperature data from a DS3231 RTC chip as well as an example characteristic that allows the client to change the state of LED diodes (writing "1" - ON, writing "0" - OFF)
To be able to interact with the server you need to authenticate by entering the server's pin when asked.
## Usage

Before flashing a device with this code, declare a "secrets.h" file in the include folder with the following structure

## Documentation

### BleServer.cpp/h
Initializes the server and sets the device name and auth pin.
Also contains a startAdvertising method to make the server known to clients
and a ServerCallbacks class to react to client connecting etc.

### MainService.cpp/h
An example service holding its characteristics.
Also contains its characteristic's callbacks to react, for example, to writes to that specific characteristic.

### Task.cpp/h
A base class representing a FreeRTOS task.
The class that inherits from this base class must call the start() method in order to start the task.
The main() method is supposed to be overridden in the child class with the task's specific logic.

### RtcTempTask.cpp/h
A class that handles the logic of reading the temperature from the DS3231's temperature sensor.