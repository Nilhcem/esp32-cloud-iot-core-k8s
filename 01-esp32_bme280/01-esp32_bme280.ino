#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_ADDRESS 0x76

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);

  Wire.begin(15, 2); // SDA, SCL
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring or BME-280 address!");
    while (1);
  }
}

void loop() {
  // Prints temperature in the Serial plotter
  Serial.println(bme.readTemperature());
  delay(10);
}
