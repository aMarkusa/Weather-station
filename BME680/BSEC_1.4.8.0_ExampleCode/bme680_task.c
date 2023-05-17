#include <stdio.h>
#include <string.h>
#include "common_porting.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "bme680_task.h"
#include "bme680.h"
#include "bme680_defs.h"
#include "bsec_datatypes.h"
#include "bsec_interface.h"
#include "bsec_integration.h"


#if defined(BSEC_IOT_EXAMPLE)
/*!
 * @brief           Write operation in either I2C or SPI
 *
 * param[in]        dev_addr        I2C or SPI device address
 * param[in]        reg_addr        register address
 * param[in]        reg_data_ptr    pointer to the data to be written
 * param[in]        data_len        number of bytes to be written
 *
 * @return          result of the bus communication function
 */
int8_t bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{
	SensorAPI_I2Cx_Write(dev_addr, reg_addr, reg_data_ptr, data_len);

	return 0;
}

/*!
 * @brief           Read operation in either I2C or SPI
 *
 * param[in]        dev_addr        I2C or SPI device address
 * param[in]        reg_addr        register address
 * param[out]       reg_data_ptr    pointer to the memory to be used to store the read data
 * param[in]        data_len        number of bytes to be read
 *
 * @return          result of the bus communication function
 */
int8_t bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{
	SensorAPI_I2Cx_Read(dev_addr, reg_addr, reg_data_ptr, data_len);

	return 0;
}

/*!
 * @brief           System specific implementation of sleep function
 *
 * @param[in]       t_ms    time in milliseconds
 *
 * @return          none
 */
void sleep(uint32_t t_ms)
{
	HAL_Delay(t_ms);
	//DelayUs(t_ms*1000);
}

/*!
 * @brief           Capture the system time in microseconds
 *
 * @return          system_current_time    current system timestamp in microseconds
 */
int64_t get_timestamp_us()
{
	int64_t system_current_time = 0;
	// ...	
	// Please insert system specific function to retrieve a timestamp (in microseconds)
	// ...
	uint32_t tick;
	tick = HAL_GetTick();
	system_current_time = 1000*(int64_t)tick;
	return system_current_time;
}

/*!
 * @brief           Handling of the ready outputs
 *
 * @param[in]       timestamp       time in nanoseconds
 * @param[in]       iaq             IAQ signal
 * @param[in]       iaq_accuracy    accuracy of IAQ signal
 * @param[in]       temperature     temperature signal
 * @param[in]       humidity        humidity signal
 * @param[in]       pressure        pressure signal
 * @param[in]       raw_temperature raw temperature signal
 * @param[in]       raw_humidity    raw humidity signal
 * @param[in]       gas             raw gas sensor signal
 * @param[in]       bsec_status     value returned by the bsec_do_steps() call
 *
 * @return          none
 */
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status,
     float static_iaq, float co2_equivalent, float breath_voc_equivalent)
{
	PDEBUG("timestamp=%lld, iaq=%f, iaq_accuracy=%d, temperature=%f, humity=%f, pressure=%f, \
		raw_temperature=%f, raw_humity=%f, gas=%f, bsec_status=%d, static_iaq=%f, co2_equivalent=%f, \
		breath_voc_equivalent=%f\r\n", timestamp, iaq, iaq_accuracy, temperature, humidity, pressure, \
		raw_temperature, raw_humidity, gas, bsec_status, static_iaq, co2_equivalent, breath_voc_equivalent);
}

/*!
 * @brief           Load previous library state from non-volatile memory
 *
 * @param[in,out]   state_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to state_buffer
 */
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer)
{
    // ...
    // Load a previous library state from non-volatile memory, if available.
    //
    // Return zero if loading was unsuccessful or no state was available, 
    // otherwise return length of loaded state string.
    // ...
    return 0;
}

/*!
 * @brief           Save library state to non-volatile memory
 *
 * @param[in]       state_buffer    buffer holding the state to be stored
 * @param[in]       length          length of the state string to be stored
 *
 * @return          none
 */
void state_save(const uint8_t *state_buffer, uint32_t length)
{
    // ...
    // Save the string some form of non-volatile memory, if possible.
    // ...
}
 
/*!
 * @brief           Load library config from non-volatile memory
 *
 * @param[in,out]   config_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to config_buffer
 */
uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer)
{
    // ...
    // Load a library config from non-volatile memory, if available.
    //
    // Return zero if loading was unsuccessful or no config was available, 
    // otherwise return length of loaded config string.
    // ...
    //memcpy(config_buffer, bsec_config_iaq, sizeof(bsec_config_iaq));
    return 0;
}


void StartBME680Task(void const * argument)
{
	return_values_init ret;

	PDEBUG("StartBME680Task\r\n");
	/* Call to the function which initializes the BSEC library 
	* Switch on low-power mode and provide no temperature offset */
	ret = bsec_iot_init(BSEC_SAMPLE_RATE_ULP, 0.0f, bus_write, bus_read, sleep, state_load, config_load);
	if (ret.bme680_status)
	{
		/* Could not intialize BME680 */
		PDEBUG("Could not intialize BSEC library, bme680_status=%d\r\n", ret.bme680_status);
		//return (int)ret.bme680_status;
	}
	else if (ret.bsec_status)
	{
		/* Could not intialize BSEC library */
		PDEBUG("Could not intialize BSEC library, bsec_status=%d\r\n", ret.bsec_status);
		//return (int)ret.bsec_status;
	}

	if((ret.bme680_status == 0) && (ret.bsec_status == 0))
	{
		PDEBUG("intialize BSEC library successful\r\n");
		/* Call to endless loop function which reads and processes data based on sensor settings */
		/* State is saved every 10000 samples, which means every 10000 * 3 secs = 500 minutes  */
		bsec_iot_loop(sleep, get_timestamp_us, output_ready, state_save, 10000);
	}

}
#endif
