# ESP32, Cloud IoT Core, Kubernetes

Blog post: [http://nilhcem.com/iot/cloud-iot-core-with-the-esp32-and-arduino](http://nilhcem.com/iot/cloud-iot-core-with-the-esp32-and-arduino)

## Projects

- `01-esp32_bme280`: ESP32 Arduino code that writes BME280 sensor data to the Serial (115200)
- `02-esp32_bme280_ciotc_arduino`: ESP32 Arduino code that sends BME280 sensor data to Google Cloud IoT Core
- `03-esp32_bme280_ciotc_esp-idf`: ESP-IDF (FreeRTOS) code for the ESP32 that sends BME280 sensor data to Google Cloud IoT Core, using the Cloud IoT Device SDK
- `04-generate-jwt`: Python3 script to generate a Json Web Token
- `05-influxdb_grafana_k8s`: Kubernetes files to host InfluxDB + Grafana
- `06-cloud_function`: Google Cloud Function that persists sensor data to InfluxDB
