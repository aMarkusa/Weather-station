#ifndef _GRAPHICS_LIBRARY_
#define _GRAPHICS_LIBRARY_

#include <stdlib.h>
#include <stdint.h>

#define SCREEN_WIDTH                400
#define SCREEN_HEIGHT               300
#define FONT_SIZE            				4 
#define CHAR_WIDTH                  (8 * FONT_SIZE) 
#define CHAR_HEIGHT                 (8 * FONT_SIZE)
#define BYTESPERROW                 (SCREEN_WIDTH / 8) 
#define BYTESPERCHARROW             (BYTESPERROW * 8) 
#define CHARSPERROW                 (SCREEN_WIDTH / CHAR_WIDTH)
#define ROWSPERCOLUMN               (SCREEN_HEIGHT / CHAR_HEIGHT)
#define LEFTEDGEBYTE              	2

#define FONT_ARRAY_LEN          22



void set_pixel(uint8_t x, uint8_t y);

//void draw_empty_row(void);


uint8_t custom_hash(char* letter);
#endif
