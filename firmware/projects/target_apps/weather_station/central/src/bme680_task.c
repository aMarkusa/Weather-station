#include "bme680_task.h"


struct bme68x_dev sensor;
struct bme68x_conf sensor_conf;
struct bme68x_heatr_conf sensor_heatr_conf;
struct bme68x_data data;


uint32_t begin_reading(){
  /* Calculate delay period in microseconds */
  uint32_t delayus_period = (uint32_t)bme68x_get_meas_dur(BME68X_FORCED_MODE, &sensor_conf, &sensor) +
                            ((uint32_t)sensor_heatr_conf.heatr_dur * 1000);

	return delayus_period;
}

bool perform_reading(){
	uint8_t n_fields;
	
	int8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &sensor);
	uint32_t reading_duration = begin_reading();
	if(reading_duration == 0){
		return false;
	}
	// wait for reading_duration to end
	delay_usec(reading_duration, NULL);
	
	// get reading data
	//uint8_t test;
	//i2c_read(0x22, &test, 1, NULL);
	//i2c_read(0x23, &test, 1, NULL);
	rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &sensor);
	if(rslt != BME68X_OK){
		return false;
	}
	else{
		return true;
	}
}

int8_t main_task(){	
	sensor.chip_id = 0x61;
  sensor.intf = BME68X_I2C_INTF;
  sensor.intf_ptr = NULL;
  sensor.read = &i2c_read;
  sensor.write = &i2c_write;
  sensor.amb_temp = 25; /* The ambient temperature in deg C is used for
																//defining the heater temperature */
  sensor.delay_us = &delay_usec;
	if(bme68x_init(&sensor) != 0){
		return 1;
	}
	
	
	sensor_conf.os_temp = BME68X_OS_2X;
	sensor_conf.os_hum = BME68X_OS_16X;
	sensor_conf.os_pres = BME68X_OS_1X;
	sensor_conf.odr = BME68X_ODR_NONE;
	sensor_conf.filter = BME68X_FILTER_OFF;
	int8_t rslt = bme68x_set_conf(&sensor_conf, &sensor);
	if(rslt != BME68X_OK){
		return 1;
	}
	sensor_heatr_conf.enable = BME68X_ENABLE;
	sensor_heatr_conf.heatr_temp = 300;
	sensor_heatr_conf.heatr_dur = 100;
	rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &sensor_heatr_conf, &sensor);
	if(rslt != BME68X_OK){
		return 1;
	}
	
	if(!perform_reading()){
		return 1;
	}
	
	else{
		/*printf("Temperature: %f\n", data.temperature);
		printf("Humidity: %f\n", data.humidity);
		printf("Pressure: %f\n", data.pressure);
		*/
		return 0;
	}
			
}

