#include "display_driver.h"
#include "user_periph_setup.h"
#include "spi.h"
#include "systick.h"
#include "io_expander.h"
#include <stdio.h>
#include "graphics_library.h"

char font[FONT_ARRAY_LEN][CHAR_HEIGHT] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
    { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
    { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
    { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
    { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
    { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
    { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
    { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
    { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
    { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
    { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
    { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
    { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
    { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
    
};

// Configuration struct for SPI
static const spi_cfg_t spi_cfg = {
	
  .spi_ms = SPI_MS_MODE_MASTER,
  .spi_cp = SPI_CP_MODE_0,
  .spi_speed = SPI_SPEED_MODE_2MHz,
  .spi_wsz = SPI_MODE_8BIT,
  .spi_cs = SPI_CS_0,
  .cs_pad.port = DISPL_SPI_PORT,
  .cs_pad.pin = DISPL_CS_PIN,
  #if defined(CFG_SPI_DMA_SUPPORT)
  .spi_dma_channel = SPI_DMA_CHANNEL_01,
  .spi_dma_priority = DMA_PRIO_0,
  #endif
};

uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void display_send_index(uint8_t index){
	GPIO_SetInactive(DISPL_SPI_PORT, DISPL_DC_PIN);
	spi_cs_low();
	spi_send(&index, 1, SPI_OP_BLOCKING);
	spi_cs_high();
	GPIO_SetActive(DISPL_SPI_PORT, DISPL_DC_PIN);
}

void display_send_data(uint8_t data){
	spi_cs_low();
	spi_send(&data, 1, SPI_OP_BLOCKING);
	spi_cs_high();
}

void display_init(){
	spi_initialize(&spi_cfg);
	
	GPIO_ConfigurePin(DISPL_SPI_PORT, DISPL_CS_PIN, OUTPUT, PID_SPI_EN, true);
	GPIO_ConfigurePin(DISPL_SPI_PORT, DISPL_SCK_PIN, OUTPUT, PID_SPI_CLK, false);
	GPIO_ConfigurePin(DISPL_SPI_PORT, DISPL_SDO_PIN, OUTPUT, PID_SPI_DO, false);
	GPIO_ConfigurePin(DISPL_SPI_PORT, DISPL_DC_PIN, OUTPUT, PID_GPIO, true);
	
}

void display_power_on(){
	displ_rst_high();
	systick_wait(5000);
	displ_rst_low();
	systick_wait(10000);
	displ_rst_high();
	systick_wait(5000);
	
	display_send_index(0x00);
	display_send_data(0x0E);
	systick_wait(5000);
}

void display_config(){
	display_send_index(0xE5);
	display_send_data(0x19);
	
	display_send_index(0xE0);
	display_send_data(0x02);
	
	display_send_index(0x00);
	display_send_data(0x0F);
	display_send_data(0x89);
}

void draw_string(char* string){
	uint8_t char_val;
	uint8_t data;
  for (uint8_t i = 0; i < CHAR_HEIGHT; i++) {
    for (uint8_t j = 0; j < CHARSPERROW; j++) {
      if (j >= LEFTEDGEBYTE && *string != '\0') {
        char_val = custom_hash(string);
				data = font[char_val][i];
        display_send_data(reverse(data));
        string++;
      } 
      else {
        display_send_data(0x00);
      }
    }
    string -= 21 ;
  }	
}

void display_send_image(){
	display_send_index(0x10);
	char* string = "%.0123456789:CINOPTUa";
	draw_empty_row(10);
	draw_string(string);
	draw_empty_row(10);
	draw_string(string);
	draw_empty_row(10);
	draw_string(string);
	draw_empty_row(4);
	draw_string(string);
	
	for (uint8_t i = 1; i <= 200; i++){
		display_send_data(0x00);
	}
	
	display_send_index(0x13);
	for (uint16_t i = 1; i <= 15000; i++){
		display_send_data(0x00);		
	}
}

void draw_empty_row(uint8_t number_of_rows){
	for (uint8_t i = 1; i <= number_of_rows; i++){
		for (uint16_t i = 1; i <= BYTESPERCHARROW; i++) {
			display_send_data(0x00);	
		}
	}
}	

void display_update_image(){
	while(!displ_busy_read());
	display_send_index(0x04);
	while(!displ_busy_read());
	display_send_index(0x12);
	while(!displ_busy_read());
 	display_send_index(0x02);
}
