#include "bme680_porting.h"
#include "i2c.h"
#include "systick.h"


// read from BME680
int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr){
	i2c_abort_t abrt_code;
	
	// enter bme680 Read-mode
	i2c_master_transmit_buffer_sync(&reg_addr, 1, &abrt_code, I2C_F_ADD_STOP);
	//delay_usec(100000, NULL);
	// read register
	i2c_master_receive_buffer_sync(reg_data_ptr, data_len, &abrt_code, I2C_F_ADD_STOP);
	
	return abrt_code;
}

// write to BME680
int8_t i2c_write(uint8_t reg_addr, uint8_t *reg_data_ptr, uint32_t data_len, void* intf_ptr){
	i2c_abort_t abrt_code;
	//uint8_t write_mode_reg = 0xEC;
	i2c_master_transmit_buffer_sync(&reg_addr, 1, &abrt_code, I2C_F_NONE);
	//delay_usec(100000, NULL);
	i2c_master_transmit_buffer_sync(reg_data_ptr, data_len, &abrt_code, I2C_F_ADD_STOP);
	
	return abrt_code;
}

// get timestampt in us
int64_t get_timestamp_us(){
	return 0;
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


