#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LCD_ADDR 0x27

// --- Public Function Prototypes ---
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
void lcd_clear(void);
void lcd_display_slots(uint8_t slots[], uint8_t total);
void lcd_print_number(uint16_t number);

#endif
