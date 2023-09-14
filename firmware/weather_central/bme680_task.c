#include "bme680_task.h"
#include "i2c.h"
#include "user_periph_setup.h"


struct bme68x_dev sensor;
struct bme68x_conf sensor_conf;
struct bme68x_heatr_conf sensor_heatr_conf;
struct bme68x_data data;

static const i2c_cfg_t bme_i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = BME_I2C_ADDR,
    .tx_fifo_level = 16,
    .rx_fifo_level = 16,
};



uint32_t begin_reading(){
  /* Calculate delay period in microseconds */
  uint32_t delayus_period = (uint32_t)bme68x_get_meas_dur(BME68X_FORCED_MODE, &sensor_conf, &sensor) +
                            ((uint32_t)sensor_heatr_conf.heatr_dur * 1000);

	return delayus_period;
}

int8_t bme680_get_data(struct bme68x_data *data){
	bme680_init();
	
	uint8_t n_fields;
	
	int8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &sensor);
	uint32_t reading_duration = begin_reading();
	if(reading_duration == 0){
		return false;
	}
	// wait for reading_duration to end
	delay_usec(reading_duration, NULL);
	
	rslt = bme68x_get_data(BME68X_FORCED_MODE, data, &n_fields, &sensor);

	i2c_release();
	return rslt;
}

int8_t bme680_init(){	
	GPIO_ConfigurePin(I2C_PORT, I2C_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, true);
	GPIO_ConfigurePin(I2C_PORT, I2C_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, true);
	i2c_init(&bme_i2c_cfg);
	
	sensor.chip_id = 0x61;
  sensor.intf = BME68X_I2C_INTF;
  sensor.intf_ptr = NULL;
  sensor.read = &i2c_read;
  sensor.write = &i2c_write;
  sensor.amb_temp = 25; /* The ambient temperature in deg C is used for
																//defining the heater temperature */
  sensor.delay_us = &delay_usec;
	int8_t rslt = bme68x_init(&sensor);
	if(rslt != BME68X_OK){
		return rslt;
	}
	sensor_conf.os_temp = BME68X_OS_2X;
	sensor_conf.os_hum = BME68X_OS_16X;
	sensor_conf.os_pres = BME68X_OS_1X;
	sensor_conf.odr = BME68X_ODR_NONE;
	sensor_conf.filter = BME68X_FILTER_OFF;
	
	rslt = bme68x_set_conf(&sensor_conf, &sensor);
	if(rslt != BME68X_OK){
		return rslt;
	}
	sensor_heatr_conf.enable = BME68X_ENABLE;
	sensor_heatr_conf.heatr_temp = 300;
	sensor_heatr_conf.heatr_dur = 100;
	
	rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &sensor_heatr_conf, &sensor);
		
	return rslt;
}

