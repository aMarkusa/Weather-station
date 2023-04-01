//#include <stdint.h>
#include "i2c.h"
#include "systick.h"
#include "sts40.h"


float read_temp(){
	GPIO_SetActive(SENSOR_POWER_PORT, SENSOR_POWER_PIN);
	systick_wait(100000);
	uint8_t write_buffer[] = {0xFD}; // temp_register
	uint8_t read_buffer[3] = {0,0,0}; 
	i2c_abort_t abort_code;
	//printf("UPDATING\n");
	i2c_master_transmit_buffer_sync(&write_buffer[0], 1, &abort_code, I2C_F_ADD_STOP);
	systick_wait(100000);
	if(abort_code == I2C_ABORT_NONE){
		printf("Write successful\n");
		i2c_master_receive_buffer_sync(read_buffer, 3, &abort_code, I2C_F_ADD_STOP);
		GPIO_SetInactive(SENSOR_POWER_PORT, SENSOR_POWER_PIN);

		float temperature = temp_to_c(read_buffer);
		printf("%f\n", temperature);
			
		return temperature;
	}
	
	else{
		return 0;
	}
}

float temp_to_c(uint8_t* raw_temp_array){
		float t_ticks = (float)raw_temp_array[0]*256.0f + (float)raw_temp_array[1];
		
	
		return t_ticks*175.0f/65535.0f - 45.0f;
}
