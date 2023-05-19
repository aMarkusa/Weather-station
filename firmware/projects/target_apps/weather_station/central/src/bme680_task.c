#include "bme680_task.h"

int8_t main_task(){
	struct bme68x_dev sensor;
	struct bme68x_conf sensor_conf;
	struct bme68x_heatr_conf sensor_heatr_conf;
	
	sensor.chip_id = 0x76;
  sensor.intf = BME68X_I2C_INTF;
  sensor.intf_ptr = NULL;
  sensor.read = &i2c_read;
  sensor.write = &i2c_write;
  sensor.amb_temp = 25; /* The ambient temperature in deg C is used for
																defining the heater temperature */
  sensor.delay_us = &delay_usec;
	int8_t rslt = bme68x_init(&sensor);
	printf("Init result: %d\n", rslt);
	
	sensor_conf.os_temp = BME680_OS_8X;
	sensor_conf.os_hum = BME680_OS_2X;
	sensor_conf.os_pres = BME680_OS_4X;
	sensor_conf.filter = BME680_FILTER_SIZE_3;
	
	sensor_heatr_conf.enable = BME68X_ENABLE;
	sensor_heatr_conf.heatr_temp = 320;
	sensor_heatr_conf.heatr_dur = 150;
	
	
	return rslt;
	
}
