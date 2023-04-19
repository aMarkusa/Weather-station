
#include <stdint.h>

// read temperature
uint16_t read_temp(void);


// Convert raw temp reading to int Ex. 25.60 C = 2560
uint16_t temp_to_int(uint8_t *raw_temp_arr);
