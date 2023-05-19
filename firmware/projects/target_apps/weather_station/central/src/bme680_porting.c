#include "bme680_porting.h"
#include "i2c.h"
#include "systick.h"


// read from BME680
int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr){
	i2c_abort_t abrt_code;
	
	// enter bme680 Read-mode
	i2c_master_transmit_buffer_sync(&reg_addr, 1, &abrt_code, I2C_F_ADD_STOP);
	delay_usec(100000, NULL);
	// read register
	i2c_master_receive_buffer_sync(reg_data_ptr, data_len, &abrt_code, I2C_F_ADD_STOP);
	
	if (abrt_code == I2C_ABORT_NONE){
		printf("BME680 data read: %d\n", *reg_data_ptr);
		return 0;
	}
	else{
		printf("BME680 data read failed\n");
		return 1;
	}
}

// write to BME680
int8_t i2c_write(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr){
	i2c_abort_t abrt_code;
	i2c_master_transmit_buffer_sync(reg_data_ptr, data_len, &abrt_code, I2C_F_ADD_STOP);
	
	if (abrt_code == I2C_ABORT_NONE){
		return 0;
	}
	else{
		return 1;
	}
}

// get timestampt in us
int64_t get_timestamp_us(){
	return 0;
}

// sensor output
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status,
     float static_iaq, float co2_equivalent, float breath_voc_equivalent){
			 
	  printf("timestamp=%lld, iaq=%f, iaq_accuracy=%d, temperature=%f, humity=%f, pressure=%f, \
		raw_temperature=%f, raw_humity=%f, gas=%f, bsec_status=%d, static_iaq=%f, co2_equivalent=%f, \
		breath_voc_equivalent=%f\r\n", timestamp, iaq, iaq_accuracy, temperature, humidity, pressure, \
		raw_temperature, raw_humidity, gas, bsec_status, static_iaq, co2_equivalent, breath_voc_equivalent);
}

		 
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer){
	return 0;
}
		 
void state_save(const uint8_t *state_buffer, uint32_t length){}

uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer){
	return 0;
}

void delay_usec(uint32_t us, void* intf_ptr){
	systick_wait(us);
}


