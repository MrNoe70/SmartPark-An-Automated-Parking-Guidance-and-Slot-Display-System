#include "gpio.h"
#include "lcd.h"
#include <util/delay.h>

// ------------------------------------------------------------
// Main Application: SmartPark Display System
// ------------------------------------------------------------
// System flow:
// 1. Initialize LCD and show startup screen
// 2. After 2s, show static parking slot statuses (P0–P5)
// ------------------------------------------------------------
int main(void) {
    lcd_init(); // Initialize LCD and I2C

    // --- Step 1: Show startup information ---
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("SmartPark System");
    lcd_set_cursor(1, 0);
    lcd_print("Initializing...");
    _delay_ms(2000);

    // --- Step 2: Show parking slot statuses ---
    lcd_clear();

    // Define 6 slots: all available (0 = available, 1 = occupied)
    uint8_t slots[6] = {1, 1, 1, 1, 1, 0};

    while (1) {
        // Continuously display static slot statuses
        lcd_display_slots(slots, 6);
        _delay_ms(1000);
    }
}
