#ifndef _IO_EXPANDER_
#define _IO_EXPANDER_

#include "i2c.h"
#include "user_periph_setup.h"

#define I2C_ADDRESS_MODE        I2C_ADDRESSING_7B
#define I2C_SPEED_MODE					I2C_SPEED_STANDARD
#define IO_EXPANDER_I2C_ADDR		(0x41)




void init_io(void);

uint8_t io_register_read(uint8_t reg_addr);

void io_register_write(uint8_t reg_addr, uint8_t data);

void displ_rst_high(void);
void displ_rst_low(void);

uint8_t displ_busy_read(void);

#endif
