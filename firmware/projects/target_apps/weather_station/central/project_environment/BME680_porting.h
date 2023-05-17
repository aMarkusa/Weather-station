#ifndef BME680_I2C
#define BME680_I2C

#include <stdint.h>
#include "bme68x.h"
#include "bme68x_defs.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"
#include "bsec_integration.h"

// read from BME680
int8_t i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len);

// write to BME680
int8_t bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len);

// get timestampt in us
int64_t get_timestamp_us(void);

// sensor output
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status,
     float static_iaq, float co2_equivalent, float breath_voc_equivalent);

		 
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer);
		 
void state_save(const uint8_t *state_buffer, uint32_t length);

uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer);

void sleep(uint32_t t_ms);

void StartBME680Task(void const * argument);
		 
#endif
		 