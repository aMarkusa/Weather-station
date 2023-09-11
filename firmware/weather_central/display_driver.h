#ifndef _DISPLAY_DRIVER_
#define _DISPLAY_DRIVER_

#include <stdint.h>
#include "bme680_task.h"

#define SCREEN_WIDTH                400
#define SCREEN_HEIGHT               300
#define FONT_SIZE            				2
#define CHAR_WIDTH                  8 
#define CHAR_HEIGHT                 8
#define BYTESPERROW                 (SCREEN_WIDTH / 8) 
#define BYTESPERCHARROW             (BYTESPERROW * 8) 
#define CHARSPERROW                 (SCREEN_WIDTH / CHAR_WIDTH)
#define ROWSPERCOLUMN               (SCREEN_HEIGHT / CHAR_HEIGHT)
#define LEFTEDGEBYTE              	2

#define FONT_ARRAY_LEN          		23

uint8_t reverse(uint8_t b);

uint8_t get_font_index(char* letter);

uint8_t* scale_byte(uint8_t byte);

void display_init(void);

void display_power_on(void);

void display_config(void);

void display_send_index(uint8_t index);

void display_send_data(uint8_t data);

void draw_string(char* string, uint8_t len);

void draw_empty_row(uint8_t number_of_rows);

void display_send_image(struct environment_data* measured_data);

void display_update_image(void);

#endif
