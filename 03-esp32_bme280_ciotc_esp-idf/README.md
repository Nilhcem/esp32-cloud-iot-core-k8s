# ESP32 BME280 Google Cloud IoT Core (ESP-IDF)

ESP-IDF FreeRTOS code for the ESP32 that sends BME280 sensor data to Google Cloud IoT Core, using the Cloud IoT Device SDK

## Dependencies

- Pull Cloud IoT Device SDK sources, running:

```
git submodule update
```

- Download and install ESP-IDF
  [https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)


## Setup Cloud IoT Core & Wiring

See `../02-esp32_bme280_ciotc_arduino/README.md`.


## Installing private key

Copy the Cloud IoT Core device private key to `main/certs/private_key.pem` subdirectory.


## Compile time constants

- Run `make menuconfig`
- Under `Wifi Configuration`, set "WiFi SSID" and "WiFI Password"
- Under `Google IoT Core Configuration`, set "Project ID", "Location", "Registry ID" and "Device ID"
- Under `BME280 Configuration`, set "SDA pin", "SCL pin" and "I2C address"

### Flash & Run

```sh
make -j4 flash monitor
```
