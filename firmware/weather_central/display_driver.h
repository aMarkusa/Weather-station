#ifndef _DISPLAY_DRIVER_
#define _DISPLAY_DRIVER_

#include <stdint.h>

uint8_t reverse(uint8_t b);

void display_init(void);

void display_power_on(void);

void display_config(void);

void display_send_index(uint8_t index);

void display_send_data(uint8_t data);

void draw_string(char* string);

void draw_empty_row(uint8_t number_of_rows);

void display_send_image(void);

void display_update_image(void);

#endif
