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


/****************************************************************************************
* Selects the sleep mode that the device will enter after advertising completion        *
*  - CFG_APP_GOTO_DEEP_SLEEP        Deep Sleep mode                                     *
*  - CFG_APP_GOTO_HIBERNATION       Hibernation mode (DA14531 only)                     *
*  - CFG_APP_GOTO_STATEFUL_HIBERNATION   Stateful Hibernation mode (DA14531 only)       *
* Note: If none is defined, then system will enter the selected Extended sleep mode     *
*                                                                                       *
* IMPORTANT: If CFG_APP_GOTO_STATEFUL_HIBERNATION is defined, CFG_STATEFUL_HIBERNATION  *
* shall be defined as well (in Options for Target 'DA14531' --> Asm -->                 *
* Conditional Assembly Control Symbols --> Define:)                                     *
****************************************************************************************/
#define CFG_APP_GOTO_DEEP_SLEEP

#if defined (__DA14531__)
#undef CFG_APP_GOTO_HIBERNATION
#undef CFG_APP_GOTO_STATEFUL_HIBERNATION

#if defined (CFG_APP_GOTO_HIBERNATION)
/****************************************************************************************
 * Hibernation mode configuration                                                       *
 ****************************************************************************************/
#define CFG_HIBERNATION_RAM1            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM2            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_RAM3            PD_SYS_DOWN_RAM_OFF
#define CFG_HIBERNATION_REMAP           REMAP_ADDR0_TO_ROM
#define CFG_HIBERNATION_PAD_LATCH_EN    false
#elif defined (CFG_APP_GOTO_STATEFUL_HIBERNATION)
/****************************************************************************************
 * Stateful Hibernation mode configuration                                              *
 ****************************************************************************************/
#define CFG_STATEFUL_HIBERNATION_RAM1           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_RAM2           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_RAM3           PD_SYS_DOWN_RAM_ON
#define CFG_STATEFUL_HIBERNATION_REMAP          STATEFUL_HIBERN_REMAP_ADDR0_TO_RAM1
#define CFG_STATEFUL_HIBERNATION_PAD_LATCH_EN   false
#endif // CFG_APP_GOTO_HIBERNATION
#endif // __DA14531__

#if defined (CFG_APP_GOTO_DEEP_SLEEP) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    #error "Config error: Can not define both CFG_APP_GOTO_DEEP_SLEEP and CFG_APP_GOTO_HIBERNATION/CFG_APP_GOTO_STATEFUL_HIBERNATION."
#endif

#if defined (CFG_SPI_FLASH_ENABLE) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    #error "Config error: Can not define both CFG_SPI_FLASH_ENABLE and CFG_APP_GOTO_HIBERNATION/CFG_APP_GOTO_STATEFUL_HIBERNATION."
#endif

#if defined (CFG_APP_GOTO_DEEP_SLEEP)
/****************************************************************************************
* Deep Sleep mode and Wake-up from Deep Sleep Configuration                             *
* Selects the trigger mechanism to wake up from deep sleep.                             *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_POR       - Wake up from POR pin                              *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_GPIO      - Wake up from WakeUp Controller                    *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_RTC       - Wake up from RTC Timer (only in DA14531)          *
*                                                                                       *
* - CFG_DEEP_SLEEP_WAKEUP_TIMER1    - Wake up from Timer1 (only in DA14531)             *
*                                                                                       *
* NOTE:                                                                                 *
* A hardware reset or power cycle will always wake up the system from deep sleep.       *
****************************************************************************************/
#undef CFG_DEEP_SLEEP_WAKEUP_POR
#undef CFG_DEEP_SLEEP_WAKEUP_GPIO

/****************************************************************************************
 * Deep sleep mode configuration                                                        *
 ****************************************************************************************/
#define CFG_DEEP_SLEEP_RAM1            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_RAM2            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_RAM3            PD_SYS_DOWN_RAM_OFF
#define CFG_DEEP_SLEEP_PAD_LATCH_EN    false

#if defined (__DA14531__)
#define CFG_DEEP_SLEEP_WAKEUP_RTC
#undef CFG_DEEP_SLEEP_WAKEUP_TIMER1
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_POR) && defined (CFG_DEEP_SLEEP_WAKEUP_GPIO)
    #error "Config error: Can not define both CFG_DEEP_SLEEP_WAKEUP_POR and CFG_DEEP_SLEEP_WAKEUP_GPIO."
#endif

#if !defined (CFG_DEEP_SLEEP_WAKEUP_POR) && !defined (CFG_DEEP_SLEEP_WAKEUP_GPIO) && \
    !defined (CFG_DEEP_SLEEP_WAKEUP_RTC) && !defined (CFG_DEEP_SLEEP_WAKEUP_TIMER1)
    #error "Config error: At least one wake-up source has to be selected."
#endif

#endif

/****************************************************************************************
* Wake-up from Extended Sleep Configuration                                             *
* Selects the trigger mechanism to wake up from extended sleep.                         *
*                                                                                       *
* - CFG_EXT_SLEEP_WAKEUP_RTC       - Wake up from RTC Timer (only in DA14531)           *
*                                                                                       *
* - CFG_EXT_SLEEP_WAKEUP_TIMER1    - Wake up from Timer1 (only in DA14531)              *
*                                                                                       *
* NOTE:                                                                                 *
* If none is defined, then system will wake up from extended sleep by button press.     *
****************************************************************************************/
#undef CFG_EXT_SLEEP_WAKEUP_RTC
#undef CFG_EXT_SLEEP_WAKEUP_TIMER1

#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) && defined (CFG_EXT_SLEEP_WAKEUP_TIMER1)
    #error "Config error: Can not define both CFG_EXT_SLEEP_WAKEUP_RTC and CFG_EXT_SLEEP_WAKEUP_TIMER1."
#endif


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
