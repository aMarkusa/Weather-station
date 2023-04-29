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
#include "user_peripheral.h"
#include "arch.h"
#include "arch_api.h"
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "sts40.h"

#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
#include "arch_hibernation.h"
#endif
#include "app_task.h"
#include "app_env.h"

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

#define TESTING_MODE

static void configure_rtc_wakeup(void);
void app_set_temp_val(uint16_t* temp_val);
static void put_system_into_deep_sleep(void);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */



void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
		if(param->reason == 0x13){
			printf("Connection terminated by central\n");
			put_system_into_deep_sleep();
		}
		else{
			default_app_on_disconnect(NULL);
		}
}

void user_app_on_connection(uint8_t conidx, struct gapc_connection_req_ind const *param)
{
#ifndef TESTING_MODE
		// set timer for next wakeup
		configure_rtc_wakeup();
#endif
		default_app_on_connection(conidx, param);
}

void user_app_on_set_dev_config_complete(void)
{
		EventRecorderInitialize(EventRecordAll, 1);
		uint16_t temp_val = read_temp();
	// read temperature
		printf("Temperature read: %d\n", temp_val);
		
		app_set_temp_val(&temp_val);
    default_app_on_set_dev_config_complete();
}

void user_app_on_init()
{
	default_app_on_init();
}



#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) || defined (CFG_DEEP_SLEEP_WAKEUP_RTC)
/**
 ****************************************************************************************
 * @brief RTC interrupt handler routine for wakeup.
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
static void configure_rtc_wakeup(void)
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
#endif


#if defined (CFG_APP_GOTO_DEEP_SLEEP)
/**
 ****************************************************************************************
 * @brief Put the system into deep sleep mode. It demonstrates the deep sleep mode usage
 *        and how the system can wake up from it. The exit from the deep sleep state causes 
 *        a system reboot.
 * @note  The system can wake up from deep sleep by:
 *          - external wake up interrupt, caused e.g. by button press (properly configured GPIO pin)
 *          - power on reset, caused e.g. by button press (properly configured GPIO pin)
 *          - interrupt generated from RTC
 *          - interrupt generated from Timer1
 *        When the system exits deep sleep state, the boot process is triggered.
 *        The application code has to be programmed in an external memory resource or
 *        in the OTP memory, in order for the system to reboot properly.
 ****************************************************************************************
*/
static void put_system_into_deep_sleep(void)
{
		// Ensure PD_TIM is open
		SetBits16(PMU_CTRL_REG, TIM_SLEEP, 0);
		// Wait until PD_TIM is opened
		while ((GetWord16(SYS_STAT_REG) & TIM_IS_UP) != TIM_IS_UP);
		
		spi_flash_power_down();

#ifdef TESTING_MODE 
    configure_rtc_wakeup();
#endif


    // Go to deep sleep
    arch_set_deep_sleep(CFG_DEEP_SLEEP_RAM1,
                        CFG_DEEP_SLEEP_RAM2,
                        CFG_DEEP_SLEEP_RAM3,
                        CFG_DEEP_SLEEP_PAD_LATCH_EN);
}
#endif // (CFG_APP_GOTO_DEEP_SLEEP)


void app_advertise_complete(const uint8_t status)
{
    if ((status == GAP_ERR_NO_ERROR) || (status == GAP_ERR_CANCELED))
    {
			// restart adv
			default_app_on_disconnect(NULL);
    }

    if (status == GAP_ERR_CANCELED)
    {
        put_system_into_deep_sleep();
		}

}

/// @} APP
