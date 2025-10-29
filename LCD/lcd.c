#include "lcd.h"
#include <util/twi.h>
#include <util/delay.h>

// ------------------------------------------------------------
// Constants for controlling LCD via PCF8574 I2C expander
// ------------------------------------------------------------
#define LCD_BACKLIGHT 0x08   // Keeps backlight ON permanently
#define LCD_ENABLE    0x04   // Enable pin bit mask
#define LCD_RS        0x01   // Register Select (0 = Command, 1 = Data)

// ------------------------------------------------------------
// I2C (TWI) Low-Level Helper Functions
// These provide manual control of start, stop, and data transmission
// ------------------------------------------------------------

// Send a START condition to begin I2C communication
static void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));  // Wait until start completes
}

// Send a STOP condition to end I2C communication
static void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    _delay_us(10);  // Small delay ensures bus release
}

// Write one byte to the I2C bus
static void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));  // Wait for write completion
}

// ------------------------------------------------------------
// LCD Transmission Helpers
// These abstract 4-bit communication to the LCD over I2C
// ------------------------------------------------------------

// Sends a half-byte (nibble) with control bits
static void lcd_send_nibble(uint8_t nibble, uint8_t control) {
    uint8_t data = nibble | control | LCD_BACKLIGHT;  // Backlight always ON
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | LCD_ENABLE);  // Pulse the Enable line (E = 1)
    _delay_us(1);
    i2c_write(data & ~LCD_ENABLE); // Latch command/data (E = 0)
    i2c_stop();
}

// Sends a full byte by splitting it into two nibbles
static void lcd_send_byte(uint8_t byte, uint8_t control) {
    lcd_send_nibble(byte & 0xF0, control);           // Send high nibble
    lcd_send_nibble((byte << 4) & 0xF0, control);    // Send low nibble
}

// ------------------------------------------------------------
// LCD Core Functions
// ------------------------------------------------------------

// Send a command (RS = 0) to configure LCD
void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0x00);
    if (cmd == 0x01 || cmd == 0x02)
        _delay_ms(2); // Longer wait for clear/home commands
}

// Send data (RS = 1) to display a character
void lcd_data(uint8_t data) {
    lcd_send_byte(data, LCD_RS);
}

// Initialize the LCD in 4-bit I2C mode
void lcd_init(void) {
    // Initialize I2C clock speed (100kHz standard mode)
    TWSR = 0x00;
    TWBR = 0x48;  // F_SCL = F_CPU / (16 + 2*TWBR) = 100kHz @ 16MHz
    _delay_ms(50); // Wait for LCD power-up

    // 4-bit initialization sequence per HD44780 datasheet
    lcd_send_nibble(0x30, 0x00); _delay_ms(5);
    lcd_send_nibble(0x30, 0x00); _delay_us(150);
    lcd_send_nibble(0x30, 0x00); _delay_us(150);
    lcd_send_nibble(0x20, 0x00); // Set to 4-bit mode

    // Function Set: 4-bit mode, 2 lines, 5x8 font
    lcd_command(0x28);
    // Display Control: Display ON, Cursor OFF, Blink OFF
    lcd_command(0x0C);
    // Entry Mode: Auto increment cursor, no display shift
    lcd_command(0x06);
    // Clear Display
    lcd_command(0x01);
    _delay_ms(2);
}

// Move cursor to specific row/column (0-based)
void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t pos = (row == 0 ? 0x00 : 0x40) + col;
    lcd_command(0x80 | pos);
}

// Print a string on the LCD
void lcd_print(const char *str) {
    while (*str) lcd_data(*str++);
}

// Clear the LCD screen
void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

// ------------------------------------------------------------
// SmartPark Slot Display Function
// ------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Function: lcd_display_slots
// Description:
//   Displays the parking slot status on a 16x2 I2C LCD.
//   Each slot (P0–P5) shows whether it’s available (0) or occupied (1).
//   When all slots are occupied, it continuously displays "FULL PARKING"
//   until any slot becomes available again.
// Parameters:
//   - slots[] : Array of uint8_t containing slot statuses (0 or 1).
//   - total   : Number of parking slots to display.
// -----------------------------------------------------------------------------
void lcd_display_slots(uint8_t slots[], uint8_t total) {
    static uint8_t is_full_displayed = 0; // Flag to remember if "FULL PARKING" is currently shown
    uint8_t full = 0;                     // Counter for occupied slots

    // -------------------------------------------------------------------------
    // Count how many slots are occupied
    // -------------------------------------------------------------------------
    for (uint8_t i = 0; i < total; i++) {
        full += slots[i];
    }

    // -------------------------------------------------------------------------
    // Case 1: All slots are full → show "FULL PARKING" continuously
    // -------------------------------------------------------------------------
    if (full == total) {
        // Only update the display once when transitioning to FULL PARKING mode
        if (!is_full_displayed) {
            lcd_clear();
            lcd_set_cursor(0, 2);
            lcd_print("FULL PARKING");
            is_full_displayed = 1; // Mark that full display is active
        }
        return; // Skip normal slot display while parking is full
    }

    // -------------------------------------------------------------------------
    // Case 2: There’s at least one available slot → show normal slot view
    // -------------------------------------------------------------------------
    is_full_displayed = 0;   // Reset flag since not all slots are full
    lcd_clear();             // Clear the LCD before writing new data
    lcd_set_cursor(0, 0);    // Start at the top-left corner (row 0, col 0)

    // -------------------------------------------------------------------------
    // Display each slot with its status
    // -------------------------------------------------------------------------
    for (uint8_t i = 0; i < total; i++) {
        lcd_print("P");           // Print slot prefix (e.g., P0)
        lcd_data('0' + i);        // Print slot number
        lcd_print(":");           // Separator
        lcd_data('0' + slots[i]); // Print status (0 or 1)
        lcd_print(" ");           // Add spacing for clarity

        // Move to second line after the third slot to fit 6 total (3 per row)
        if (i == 2)
            lcd_set_cursor(1, 0);
    }
}

