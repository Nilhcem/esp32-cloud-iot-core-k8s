# ESP32, Cloud IoT Core, Kubernetes

Blog post: [http://nilhcem.com/iot/cloud-iot-core-with-the-esp32](http://nilhcem.com/iot/cloud-iot-core-with-the-esp32)

## Projects

- `01-esp32_bme280`: ESP32 Arduino code that writes BME280 sensor data to the Serial (115200)
- `02-esp32_bme280_ciotc`: ESP32 Arduino code that sends BME280 sensor data to Google Cloud IoT Core
- `03-generate-jwt`: Python3 script to generate a Json Web Token
- `04-influxdb_grafana_k8s`: Kubernetes files to host InfluxDB + Grafana
- `05-cloud_function`: Google Cloud Function that persists sensor data to InfluxDB
