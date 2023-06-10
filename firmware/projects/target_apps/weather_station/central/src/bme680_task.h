#ifndef BME680_TASK
#define BME680_TASK

#include "bme68x.h"
#include "bme68x_defs.h"
#include "bme680_porting.h"
#include <stdbool.h>

uint8_t read_sensor(float sensor_data[4]);

uint32_t begin_reading(void);

int8_t perform_reading(void);

#endif
