# ESP32 BME280

ESP32/ESP8266 Arduino code that prints BME280 temperature to the Arduino Serial Plotter (115200)


## Dependencies

You'll need to install from the Arduino IDE those dependencies:
- **Adafruit BME280 Library** by Adafruit ([more info](https://github.com/adafruit/Adafruit_BME280_Library))
- **Adafruit Unified Sensor** by Adafruit ([more info](https://github.com/adafruit/Adafruit_Sensor))


## Wiring

The BME280 sensor should be connected to the ESP over I2C.  
If the BME280 I2C address is not `0x76`, you'll need to modify the `BME280_ADDRESS` constant.  
Depending on your board, you'll also need to modify the SDA and SCL pin names in the `Wire.begin(SDA, SCL);` call.

**Wemos Lolin D32 Pro**
- SDA: `21`
- SCL: `22`

**Wemos Lolin32 Lite**
- SDA: `15`
- SCL: `2`

**DOIT Esp32 DevKit v1**
- SDA: `21`
- SCL: `22`

**ESP8266 WeMos D1 Mini**
- SDA: `D2`
- SCL: `D1`
