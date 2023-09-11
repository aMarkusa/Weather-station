#ifndef BME680_I2C
#define BME680_I2C

#include <stdint.h>
#include "bme68x.h"
#include "bme68x_defs.h"


// read from BME680
int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr);

// write to BME680
int8_t i2c_write(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr);

// get timestampt in us
int64_t get_timestamp_us(void);

		 
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer);
		 
void state_save(const uint8_t *state_buffer, uint32_t length);

uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer);

void delay_usec(uint32_t us, void* intf_ptr);
		 
#endif
		 