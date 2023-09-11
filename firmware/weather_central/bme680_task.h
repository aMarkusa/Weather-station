#ifndef BME680_TASK
#define BME680_TASK

#include "bme68x.h"
#include "bme68x_defs.h"
#include "bme680_porting.h"
#include <stdbool.h>

struct environment_data {
	float node_temperature;
	float temperature;
	uint32_t pressure;
	float humidity;
	uint32_t air_quality;
};


int8_t bme680_init(void);

uint32_t begin_reading(void);

bool perform_reading(void);

int8_t bme680_get_data(struct bme68x_data *data);

#endif
