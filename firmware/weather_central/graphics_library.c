#include "graphics_library.h"
//include "font.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

uint8_t arr[23] = {0};
uint16_t adder = 0;
#include <stdint.h>


uint8_t custom_hash(char* letter){
    char* mapping = " %.0123456789:CINOPTUa";
    uint8_t i = 0;
    while (*mapping != '\0') {
        if (*letter == *mapping) {
            return i % 22; // Mapping to the range 0-21
        }
        mapping++;
        i++;
    }

        return -1; // Value not found in the mapping
        
}
/*
void draw_char_row(char* string, uint8_t len){
  uint8_t char_val;
  bool EOR = false;
  for (uint8_t i = 0; i < CHAR_HEIGHT; i++) {
    for (uint8_t j = 0; j < CHARSPERROW; j++) {
      if (j >= LEFTEDGEBYTE && *string != '\0') {
        char_val = custom_hash(string);
        printf("%2x ", font[char_val][i]);
        string++;
      } 
      else {
        printf("%2x ", 0);
      }
    }
    printf("\n");
    string -= len;
  }
}

void draw_empy_row(){
    for (uint16_t i = 1; i <= BYTESPERCHARROW; i++) {
        printf("0");
        fflush(stdout);
        if ((i % BYTESPERROW) == 0) {
          printf("\n");
        }
    }
}


void main() {  
    char* test = "IN OUT";
    draw_char_row(test, strlen(test));
    // while (*test != '\0') {
    //     int hashed_value = custom_hash(test);
    //     printf("Char %c maps to %d\n", *test, hashed_value);
    //     test++;
    // }
}*/
