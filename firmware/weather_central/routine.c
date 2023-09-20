/**
 ****************************************************************************************
 *
 * @file user_barebone.c
 *
 * @brief Barebone project source code.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
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
#include "gap.h"
#include "app_easy_timer.h"
#include "routine.h"
#include "co_bt.h"
#include <string.h>
#include "user_periph_setup.h"             // SW configuration

#include "systick.h"
#include "bme680_task.h"
#include "display_driver.h"
#include "io_expander.h"





/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */



/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */



/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
struct scan_configuration {
    /// Operation code.
    uint8_t     code;
    /// Own BD address source of the device
    uint8_t     addr_src;
    /// Scan interval
    uint16_t    interval;
    /// Scan window size
    uint16_t    window;
    /// Scanning mode
    uint8_t     mode;
    /// Scan filter policy
    uint8_t     filt_policy;
    /// Scan duplicate filtering policy
    uint8_t     filter_duplic;
};

static const struct scan_configuration user_scan_conf ={
    /// Operation code.
    .code = GAPM_SCAN_ACTIVE,
    /// Own BD address source of the device
    .addr_src = GAPM_CFG_ADDR_PUBLIC,
    /// Scan interval
    .interval = 100,
    /// Scan window size
    .window = 50,
    /// Scanning mode
    .mode = GAP_OBSERVER_MODE,
    /// Scan filter policy
    .filt_policy = SCAN_ALLOW_ADV_ALL,
    /// Scan duplicate filtering policy
    .filter_duplic = SCAN_FILT_DUPLIC_EN  //SCAN_FILT_DUPLIC_DIS SCAN_FILT_DUPLIC_EN
};
void user_scan_start(void);

char peripheral[] = "Temperature";

float node_temperature;

void get_sensor_data(struct environment_data* measured_data){
	struct bme68x_data data;
	
	// get sensor data
	bme680_get_data(&data);
	measured_data->node_temperature = node_temperature;
	measured_data->temperature = data.temperature;
	measured_data->pressure = data.pressure;
	measured_data->humidity = data.humidity;
	measured_data->gas_resistance = data.gas_resistance;
}


void routine(void)
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


/**
 ****************************************************************************************
 * @brief Scan function
 * @return void
 ****************************************************************************************
 */
void user_scan_start(void)
{
    struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
                            TASK_GAPM, TASK_APP,
                            gapm_start_scan_cmd);

    cmd->op.code = user_scan_conf.code;
    cmd->op.addr_src = user_scan_conf.addr_src;
    cmd->interval = user_scan_conf.interval;
    cmd->window = user_scan_conf.window;
    cmd->mode = user_scan_conf.mode;
    cmd->filt_policy = user_scan_conf.filt_policy;
    cmd->filter_duplic = user_scan_conf.filter_duplic;

    // Send the message
    ke_msg_send(cmd);

    // We are now connectable
    //ke_state_set(TASK_APP, APP_CONNECTABLE);
    
		//printf_string(UART2, "SCAN START\r\n");
    
}

void user_scan_stop(void){
	struct gapm_cancel_cmd* cmd = KE_MSG_ALLOC(GAPM_CANCEL_CMD,
                            TASK_GAPM, TASK_APP,
                            gapm_cancel_cmd);
	cmd->operation = GAPM_CANCEL;
	
	ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Called upon device configuration complete message
 * @return void
 ****************************************************************************************
 */
void user_on_set_dev_config_complete( void )
{
		// start the scanning
		//printf_string(UART2, "Dev config complete\r\n");
		user_scan_start();
}

/**
 ****************************************************************************************
 * @brief Scan complete function. Restart the scanning
 * @return void
 ****************************************************************************************
 */
void user_on_scan_complete(const uint8_t param){
    //printf_string(UART2, "SCAN COMPLETE\r\n");
		routine();
}

static inline void format_adv_string(uint8_t * data, uint8_t len, char *out_string)
{
	memcpy(out_string, data, len);
	out_string[len] = '\0';
	
}
/**
 ****************************************************************************************
 * @brief Get number of advertisement elements
 * @param[in] data - pointer to advertisement data
 * @param[in] len - data length
 * @return number of elements
 ****************************************************************************************
 */
uint8_t user_ble_gap_get_adv_num_elements(const uint8_t *data, uint8_t len)
{
	uint8_t i;
	//start with 1 to comply with BT SIG
	uint8_t num_elements = 1;
	i=data[0] + 1;
	while( i<len)
	{
		num_elements++;
		i += data[i] + 1;
		
	}
	
	return num_elements;
	
}

/**
 ****************************************************************************************
 * @brief parse advertisement data into ad_elements structure
 * @param[in] in_len - advertisement data length to parse
 * @param[in] in_data - data to parse
 * @param[out] ad_elements - parsed data
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_parse_adv_data(uint8_t in_len, const uint8_t *in_data, ble_gap_adv_struct_t *ad_elements)
{

	uint8_t i = 0;
	uint8_t elem_count = 0;
	while( i<in_len)
	{
		ad_elements[elem_count].len = 	in_data[i] - 1;
		ad_elements[elem_count].type = 	in_data[i+1];
		ad_elements[elem_count].data = 	(uint8_t *)in_data + i + 2;
		i += in_data[i] + 1;
		elem_count++;
		
	}
	
}


/**
 ****************************************************************************************
 * @brief Advertising report function. Decode and display most popular advertising field
 * @return void
 ****************************************************************************************
 */
void user_on_adv_report_ind( struct gapm_adv_report_ind const *param)
{

	uint8_t num_ad_elements = user_ble_gap_get_adv_num_elements(param->report.data, 
																																		param->report.data_len);
	ble_gap_adv_struct_t adv_data_structs[num_ad_elements]; 
	
	user_ble_gap_parse_adv_data(param->report.data_len, param->report.data, adv_data_structs);
	
	uint8_t i;
	for(i = 0 ; i < num_ad_elements; i++)
	{
		
		switch(adv_data_structs[i].type)
		{
#if (SCAN_FILTER == (SCAN_FILTER_NAME)) 

			case GAP_AD_TYPE_COMPLETE_NAME:
			{
				
				
				char local_name[adv_data_structs[i].len + 1];
				format_adv_string(adv_data_structs[i].data,adv_data_structs[i].len, local_name);
				int8_t rslt = strcmp((const char*)local_name, (const char*)peripheral);
				if (rslt == 0){
					uint8_t decimal = *(adv_data_structs[i+1].data + 2);
					uint8_t integer = *(adv_data_structs[i+1].data + 3);
					
					uint16_t raw_temperature = ((0x00 | integer) << 8) | decimal;
					node_temperature = raw_temperature / 100.0;
					//printf("Device Local Name: %s\r\n", local_name);
					user_scan_stop();
					//routine();
				}
				break;
			}
#endif 

			default:
			{
#if SCAN_FILTER == SCAN_FILTER_NONE
				
				
				////printf("GAP Type: %02x, Data: %s\r\n", adv_data_structs[i].type, 
				//								format_hex_string(adv_data_structs[i].data, adv_data_structs[i].len) );
#endif
				break;
			}
		}
	}
		
	

////printf("\r\n ----------------------- \r\n");

	
}


/// @} APP
