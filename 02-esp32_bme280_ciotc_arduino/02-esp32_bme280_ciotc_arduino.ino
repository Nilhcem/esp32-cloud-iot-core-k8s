// The MQTT callback function for commands and configuration updates
// Place your message handler code here.
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

#include "universal-mqtt.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_ADDRESS 0x76
#define PUBLISH_DELAY 60000

Adafruit_BME280 bme;
float humidity;
float temperature;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(15, 2); // SDA, SCL
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring or BME-280 address!");
    while (1);
  }

  // Use force mode so that the sensor returns to sleep mode when the measurement is finished
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_NONE, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF);

  setupCloudIoT();
}

void loop() {
  mqttClient->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }

  // publish a message roughly every PUBLISH_DELAY ms.
  if (millis() - lastMillis > PUBLISH_DELAY) {
    lastMillis = millis();

    bme.takeForcedMeasurement(); // has no effect in normal mode
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("BME280 reading issues");
      return;
    }

    String payload = String("{\"timestamp\":") + time(nullptr) +
                     String(",\"temperature\":") + temperature +
                     String(",\"humidity\":") + humidity +
                     String("}");
    publishTelemetry(payload);
  }
}
