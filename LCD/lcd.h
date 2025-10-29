#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// The I2C slave address for most PCF8574-based LCD backpacks.
// This may vary depending on your specific module (e.g., 0x3F on some).
#define LCD_ADDR 0x27

// --- Public Function Prototypes ---
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
void lcd_clear(void);
void lcd_display_slots(uint8_t slots[], uint8_t total);

#endif
