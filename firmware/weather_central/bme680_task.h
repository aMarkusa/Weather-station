#ifndef BME680_TASK
#define BME680_TASK

#include "bme68x.h"
#include "bme68x_defs.h"
#include "bme680_porting.h"
#include <stdbool.h>

int8_t bme680_init(void);

uint32_t begin_reading(void);

bool perform_reading(void);

#endif
