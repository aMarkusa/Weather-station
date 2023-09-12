#include "io_expander.h"

// I2C_CFG struct
const i2c_cfg_t io_i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = IO_EXPANDER_I2C_ADDR,
    .tx_fifo_level = 16,
    .rx_fifo_level = 16,
};

void displ_rst_high(){
	io_register_write(0x01, 0xF1); 
}

void displ_rst_low(){
	io_register_write(0x01, 0xF0); 
}

uint8_t displ_busy_read(void){
	uint8_t input_register = 0;
	uint8_t busy_state = 0xff;
	input_register = io_register_read(0x00);
	busy_state = (input_register & 0x02) >> 1;
	
	return  busy_state;
}

void init_io(){
	i2c_init(&io_i2c_cfg);
	
	io_register_write(0x03, 0xFE); // set P1 to input
	io_register_write(0x50, 0x40); // disable internal pullup
	
	displ_rst_low(); // set display reset (P0) to low
}

uint8_t io_register_read(uint8_t reg_addr){
	uint8_t reg_data = 0;
	i2c_abort_t rslt;
	i2c_master_transmit_buffer_sync(&reg_addr, 1, &rslt, I2C_F_NONE);
	i2c_master_receive_buffer_sync(&reg_data, 1, &rslt, I2C_F_ADD_STOP);
	
	return reg_data;
}

void io_register_write(uint8_t reg_addr, uint8_t data){
	uint8_t buffer[] = {reg_addr, data};
	i2c_abort_t rslt;
	i2c_master_transmit_buffer_sync(buffer, 2, &rslt, I2C_F_ADD_STOP);
}
