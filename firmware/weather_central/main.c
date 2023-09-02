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



/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

 /**
 ****************************************************************************************
 * @brief User code initialization function.
 ****************************************************************************************
*/


void user_on_init(void)
{
		//EventRecorderInitialize (EventRecordAll, 1);
    arch_set_sleep_mode(app_default_sleep_mode);
	
		GPIO_SetActive(PWR_SWITCH_PORT, PWR_SWITCH_PIN);
		systick_wait(10000);
		display_init();
		display_power_on();
		display_config();
		display_send_image();
		display_update_image();
}

/// @} APP
