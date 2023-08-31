/**
 ****************************************************************************************
 *
 * @file user_proxr.c
 *
 * @brief Proximity reporter project source code.
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "gapc_task.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "app_easy_msg_utils.h"
#include "gpio.h"
#include "app_security.h"
#include "temp_node.h"
#include "arch.h"
#include "arch_api.h"
#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
#include "arch_hibernation.h"
#endif
#include "app_task.h"
#include "app_proxr.h"
#include "app_api.h"
#include "sts40.h"
#include "i2c.h"

#if defined (__DA14531__)
#include "rtc.h"
#include "timer1.h"
#endif

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
#include "spi_flash.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#define ADV_LEN		19
#define UPDATE_INTERVAL		30000 // 5min (1s = 100)
uint8_t adv_data[ADV_LEN];

// I2C_CFG struct
const i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = TEMP_I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = TEMP_I2C_ADDRESS_MODE,
    .address = TEMP_I2C_SLAVE_ADDRESS,
    .tx_fifo_level = 16,
    .rx_fifo_level = 16,
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
 
void update_adv_data()
{		
		arch_set_sleep_mode(ARCH_SLEEP_OFF);
		GPIO_ConfigurePin(TEMP_I2C_SCL_PORT, TEMP_I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(TEMP_I2C_SDA_PORT, TEMP_I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
		GPIO_ConfigurePin(SENSOR_POWER_PORT, SENSOR_POWER_PIN, OUTPUT, PID_GPIO, false);
		i2c_init(&i2c_cfg);
		// Update temperature data
		uint16_t temperature = read_temp();
		// disable i2c block
		i2c_release();
		GPIO_ConfigurePin(SENSOR_POWER_PORT, SENSOR_POWER_PIN, NULL, NULL, false);


	
		uint8_t lsb = temperature & 0xFF;
		uint8_t msb = temperature >> 8;
	

		adv_data[ADV_LEN - 2] = lsb;
		adv_data[ADV_LEN - 1] = msb;
			
   // Update the advertising data
    app_easy_gap_update_adv_data(adv_data, ADV_LEN, NULL, NULL);

   // Restart the timer
		arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);
    app_easy_timer(UPDATE_INTERVAL, update_adv_data); 
}

 void user_advertise_operation(void)
{
		GPIO_ConfigurePin(TEMP_I2C_SCL_PORT, TEMP_I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(TEMP_I2C_SDA_PORT, TEMP_I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
		GPIO_ConfigurePin(SENSOR_POWER_PORT, SENSOR_POWER_PIN, OUTPUT, PID_GPIO, false);
		i2c_init(&i2c_cfg);
		uint16_t temperature = read_temp();
		// disable i2c block
		i2c_release();
		GPIO_ConfigurePin(SENSOR_POWER_PORT, SENSOR_POWER_PIN, NULL, NULL, false);

		uint8_t lsb = temperature & 0xFF;
		uint8_t msb = temperature >> 8;
    // Copy the current default advertising data specified in user_config.h
    struct gapm_start_advertise_cmd* adv_cmd; // Retained version
    struct gapm_start_advertise_cmd* cmd;     // Version we send
    cmd = app_advertise_start_msg_create();   // Create an empty advertise start command

    // Get the advertising properties from the app_easy function
    adv_cmd = app_easy_gap_non_connectable_advertise_get_active();
    // Make a copy of the advertising properties
    memcpy(cmd, adv_cmd, sizeof(struct gapm_start_advertise_cmd));
		
    // Adv. data segment with length of 0x0C, type is BD name and Service data (temperature)
		uint8_t new_adv_data[] = {0x0C, 0x09, 0x54, 0x65, 0x6D, 0x70, 0x65, 0x72, 0x61, 0x74, 0x75, 0x72, 0x65, 0x05, 0x16, 0x6E, 0x2A, lsb, msb};  // device_name|temperature
		memcpy(adv_data, new_adv_data, ADV_LEN * sizeof(uint8_t));
	
    cmd->info.host.adv_data_len	= sizeof(adv_data);
    memcpy(&cmd->info.host.adv_data, &adv_data, sizeof(adv_data));

    // Send message to GAP task
    ke_msg_send(cmd);
		
		arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);
		app_easy_timer(UPDATE_INTERVAL, update_adv_data); 
	

}





/// @} APP
