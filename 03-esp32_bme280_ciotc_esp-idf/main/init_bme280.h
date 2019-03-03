#ifndef __INIT_BME280_H__
#define __INIT_BME280_H__

struct bme280_data
{
  float temperature;
  float humidity;
};

int init_bme280();

int get_bme280_data(struct bme280_data *data);

#endif /* __INIT_BME280_H__ */
