#ifndef COMMON_PORTING_H__
#define COMMON_PORTING_H__

#include <stdint.h>
#include "user_define.h"


#define I2C_HANDLE	(hi2c1)
#define SPI_HANDLE	(hspi1)
#define UART_HANDLE	(huart2)

#define BUS_TIMEOUT             1000

void Enable_MCU_INT1_Pin(void);
void Disable_MCU_INT1_Pin(void);
void Enable_MCU_INT2_Pin(void);
void Disable_MCU_INT2_Pin(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


void DelayUs(uint32_t Delay);
void UART_Printf(uint8_t* buff, uint16_t size);
void PDEBUG(char *format, ...);


#if defined(USE_BOSCH_SENSOR_API)
int8_t SensorAPI_I2Cx_Read(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint16_t ReadNumbr);
int8_t SensorAPI_I2Cx_Write(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint16_t WriteNumbr);
int8_t SensorAPI_SPIx_Read(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint16_t ReadNumbr);
int8_t SensorAPI_SPIx_Write(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint16_t WriteNumbr);
int8_t SensorAPI_BMA2X2_I2Cx_Read(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t ReadNumbr);
int8_t SensorAPI_BMA2X2_I2Cx_Write(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t WriteNumbr);
int8_t SensorAPI_BMA2X2_SPIx_Read(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t ReadNumbr);
int8_t SensorAPI_BMA2X2_SPIx_Write(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t WriteNumbr);
int8_t SensorAPI_BMX160_SPIx_Read(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t ReadNumbr);
int8_t SensorAPI_BMX160_SPIx_Write(uint8_t slave_address7, uint8_t subaddress, uint8_t *pBuffer, uint8_t WriteNumbr);
#endif

#endif
