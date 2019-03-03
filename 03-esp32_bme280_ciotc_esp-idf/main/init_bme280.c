#include <stdio.h>

#include "bme280.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "init_bme280.h"
#include "sdkconfig.h"

#define TAG "BME280"

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

struct bme280_t bme280;
s32 v_uncomp_pressure_s32;
s32 v_uncomp_temperature_s32;
s32 v_uncomp_humidity_s32;

void i2c_master_init()
{
  i2c_config_t i2c_config = {
			     .mode = I2C_MODE_MASTER,
			     .sda_io_num = CONFIG_SDA_PIN,
			     .scl_io_num = CONFIG_SCL_PIN,
			     .sda_pullup_en = GPIO_PULLUP_ENABLE,
			     .scl_pullup_en = GPIO_PULLUP_ENABLE,
			     .master.clk_speed = 1000000
  };
  i2c_param_config(I2C_NUM_0, &i2c_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
  s32 iError = BME280_INIT_VALUE;

  esp_err_t espRc;
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

  i2c_master_write_byte(cmd, reg_addr, true);
  i2c_master_write(cmd, reg_data, cnt, true);
  i2c_master_stop(cmd);

  espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
  if (espRc == ESP_OK) {
    iError = SUCCESS;
  } else {
    iError = FAIL;
  }
  i2c_cmd_link_delete(cmd);

  return (s8)iError;
}

s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
  s32 iError = BME280_INIT_VALUE;
  esp_err_t espRc;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg_addr, true);

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

  if (cnt > 1) {
    i2c_master_read(cmd, reg_data, cnt-1, I2C_MASTER_ACK);
  }
  i2c_master_read_byte(cmd, reg_data+cnt-1, I2C_MASTER_NACK);
  i2c_master_stop(cmd);

  espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
  if (espRc == ESP_OK) {
    iError = SUCCESS;
  } else {
    iError = FAIL;
  }

  i2c_cmd_link_delete(cmd);

  return (s8)iError;
}

void BME280_delay_msek(u32 msek)
{
  vTaskDelay(msek/portTICK_PERIOD_MS);
}

int get_bme280_data(struct bme280_data *data)
{
  s32 com_rslt = bme280_read_uncomp_pressure_temperature_humidity(&v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

  if (com_rslt == SUCCESS) {
    data->temperature = bme280_compensate_temperature_double(v_uncomp_temperature_s32);
    data->humidity = bme280_compensate_humidity_double(v_uncomp_humidity_s32);
  } else {
    ESP_LOGE(TAG, "measure error. code: %d", com_rslt);
  }
  return com_rslt == SUCCESS;
}

int init_bme280()
{
  i2c_master_init();

  bme280.bus_write = BME280_I2C_bus_write;
  bme280.bus_read = BME280_I2C_bus_read;
  bme280.dev_addr = BME280_I2C_ADDRESS1;
  bme280.delay_msec = BME280_delay_msek;

  s32 com_rslt = bme280_init(&bme280);

  com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
  com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_2X);
  com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

  com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
  com_rslt += bme280_set_filter(BME280_FILTER_COEFF_16);

  com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
  return com_rslt == SUCCESS;
}
