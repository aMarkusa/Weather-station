/**
 ****************************************************************************************
 *
 * @file user_barebone.h
 *
 * @brief Barebone application header file.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_BAREBONE_H_
#define _USER_BAREBONE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "rwble_config.h"
#include "gapm_task.h"                 // gap functions and messages
#include "co_error.h"                  // error code definitions
#include "app_api.h"
#include "arch_console.h"
#include "uart_utils.h"
#include "ke_msg.h"
#include "bme680_task.h"
//#include "user_custs1_def.h"
//#include "user_custs_config.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define SCAN_FILTER_NONE									(0)
#define SCAN_FILTER_NAME									(1)
#define SCAN_FILTER_16_BIT_SVC_DATA				(2)
#define SCAN_FILTER_TX_POWER							(3)
#define SCAN_FILTER_MFG_DATA							(4)

#define	SCAN_FILTER												(SCAN_FILTER_NAME)
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void user_on_set_dev_config_complete( void );
void user_on_adv_report_ind(const struct gapm_adv_report_ind *param);
void user_on_scan_complete(const uint8_t param);;
/**
 ****************************************************************************************
 * @brief Get number of advertisement elements
 * @param[in] data - pointer to advertisement data
 * @param[in] len - data length
 * @return number of elements
 ****************************************************************************************
 */
uint8_t user_ble_gap_get_adv_num_elements(const uint8_t *data, uint8_t len);
void get_sensor_data(struct environment_data* measured_data);
void update_data(void);

typedef struct
{
	///Payload length, BLE length byte will get calculated in helper function
	uint8_t 	len;
	///GAP Flag
	uint8_t 	type;
	///Payload data
	uint8_t 	*data;
	
}ble_gap_adv_struct_t;

/**
 ****************************************************************************************
 * @brief parse advertisement data into ad_elements structure
 * @param[in] in_len - advertisement data length to parse
 * @param[in] in_data - data to parse
 * @param[out] ad_elements - parsed data
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_parse_adv_data(uint8_t in_len, const uint8_t *in_data, ble_gap_adv_struct_t *ad_elements);

typedef enum{
	
	 BLE_GAP_ERROR_NO_ERROR,
	 BLE_GAP_ERROR_INVALID_PARAMS,
	
}ble_gap_error_t;
 


/// @} APP

#endif //_USER_BAREBONE_H_
