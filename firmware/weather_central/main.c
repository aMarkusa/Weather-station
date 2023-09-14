/**
 ****************************************************************************************
 *
 * @file user_empty_template_ext.c
 *
 * @brief Empty template project for external processor mode source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
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
#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"             // SW configuration
#include "main.h"
#include "arch_api.h"
#include "user_config.h"
#include "io_expander.h"
#include "display_driver.h"
#include <stdio.h>
//#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "systick.h"
#include "bme680_task.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

 /**
 ****************************************************************************************
 * @brief User code initialization function.
 ****************************************************************************************
*/
void get_sensor_data(struct environment_data* measured_data){
	struct bme68x_data data;
	
	// get sensor data
	bme680_get_data(&data);
	measured_data->node_temperature = 25.76;
	measured_data->temperature = data.temperature;
	measured_data->pressure = data.pressure;
	measured_data->humidity = data.humidity;
	measured_data->gas_resistance = data.gas_resistance;
}


void user_on_init(void)
{
		struct environment_data measured_data;
		//EventRecorderInitialize (EventRecordAll, 1);
    arch_set_sleep_mode(app_default_sleep_mode);
	
		GPIO_SetActive(PWR_SWITCH_PORT, PWR_SWITCH_PIN);
		systick_wait(10000);
		get_sensor_data(&measured_data);
		display_init();
		display_power_on();
		display_config();
		display_send_image(&measured_data);
		display_update_image();
}

/// @} APP
