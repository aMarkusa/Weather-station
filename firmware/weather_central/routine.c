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
#include "rtc.h"





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

static void rtc_interrupt_hdlr(uint8_t event)
{
#if defined (CFG_EXT_SLEEP_WAKEUP_RTC)
    app_resume_system_from_sleep();
#endif
}

/**
 ****************************************************************************************
 * @brief Configure RTC to generate an interrupt after 10 seconds.
 ****************************************************************************************
*/
void configure_rtc_wakeup(void)
{
    rtc_time_t alarm_time;

    // Init RTC
    rtc_reset();

    // Configure the RTC clock; RCX is the RTC clock source (14420 Hz)
    rtc_clk_config(RTC_DIV_DENOM_1000, 14420);
    rtc_clock_enable();

    rtc_config_t cfg = {.hour_clk_mode = RTC_HOUR_MODE_24H, .keep_rtc = 0};

    rtc_time_t time = {.hour_mode = RTC_HOUR_MODE_24H, .pm_flag = 0, .hour = 11,
                       .minute = 55, .sec = 30, .hsec = 00};

    // Alarm interrupt in ten seconds
    alarm_time = time;
    alarm_time.sec += 10;

    // Initialize RTC, set time and data, register interrupt handler callback function and enable seconds interrupt
    rtc_init(&cfg);

    // Start RTC
    rtc_set_time_clndr(&time, NULL);
    rtc_set_alarm(&alarm_time, NULL, RTC_ALARM_EN_SEC);

    // Clear pending interrupts
    rtc_get_event_flags();
    rtc_register_intr(rtc_interrupt_hdlr, RTC_INTR_ALRM);
#if defined (CFG_EXT_SLEEP_WAKEUP_RTC)
    app_easy_wakeup_set(app_wakeup_cb);
#endif
}

static void put_system_into_deep_sleep(void)
{
	
	// Ensure PD_TIM is open
	SetBits16(PMU_CTRL_REG, TIM_SLEEP, 0);
	// Wait until PD_TIM is opened
	while ((GetWord16(SYS_STAT_REG) & TIM_IS_UP) != TIM_IS_UP);
	
#if defined (CFG_DEEP_SLEEP_WAKEUP_POR)
    // Configure button for POR
    GPIO_EnablePorPin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO_POR_PIN_POLARITY_LOW, GPIO_GetPorTime());
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_GPIO)
    wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // Select pin
                      WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // Polarity low
                      1, // 1 event
                      0); // debouncing time = 0
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_RTC)
    configure_rtc_wakeup();
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_TIMER1)
    configure_timer1_wakeup();
#endif

    // Go to deep sleep
    arch_set_deep_sleep(CFG_DEEP_SLEEP_RAM1,
                        CFG_DEEP_SLEEP_RAM2,
                        CFG_DEEP_SLEEP_RAM3,
                        CFG_DEEP_SLEEP_PAD_LATCH_EN);
}

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


char peripheral[] = "Temperature";
float node_temperature;  // temperature from our peripheral device

void get_sensor_data(struct environment_data* measured_data){
	struct bme68x_data data;
	
	// get sensor data and copy to struct
	bme680_get_data(&data);
	measured_data->node_temperature = node_temperature;
	measured_data->temperature = data.temperature;
	measured_data->pressure = data.pressure;
	measured_data->humidity = data.humidity;
	measured_data->gas_resistance = data.gas_resistance;
}


void update_data(void)
{
		struct environment_data measured_data;
	
		GPIO_SetActive(PWR_SWITCH_PORT, PWR_SWITCH_PIN);
		systick_wait(10000); // wait for peripherals to power on
		get_sensor_data(&measured_data);
		display_init();
		display_power_on();
		display_config();
		display_send_image(&measured_data);
		display_update_image();
		GPIO_SetInactive(PWR_SWITCH_PORT, PWR_SWITCH_PIN);
	
		put_system_into_deep_sleep();
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
		update_data();
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
		if (adv_data_structs[i].type == GAP_AD_TYPE_COMPLETE_NAME){
			// check if the peripheral device is ours
			char local_name[adv_data_structs[i].len + 1];
			format_adv_string(adv_data_structs[i].data,adv_data_structs[i].len, local_name);
			int8_t rslt = strcmp((const char*)local_name, (const char*)peripheral); 
			if (rslt == 0){  // if it is ours
				// stop the scan 
				user_scan_stop();
				
				// parse the temperature data
				uint8_t decimal = *(adv_data_structs[i+1].data + 2);
				uint8_t integer = *(adv_data_structs[i+1].data + 3);
				
				uint16_t raw_temperature = ((0x00 | integer) << 8) | decimal;
				node_temperature = raw_temperature / 100.0;
			}
		}	
	}	
}


/// @} APP
