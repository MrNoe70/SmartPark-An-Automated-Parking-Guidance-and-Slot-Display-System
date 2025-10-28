#include "uart.h"

// --- Initialize UART for 9600 baud, 8N1 format ---
void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;

    // Enable TX
    UCSR0B = (1 << TXEN0);

    // 8-bit data, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Redirect printf() output to UART
    static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &uart_stdout;
}

// --- Transmit a Single Character ---
void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait until buffer is empty
    UDR0 = data;
}

// --- Transmit String ---
void uart_print(const char* str) {
    while (*str) {
        uart_transmit(*str++);
    }
}

// --- printf() Hook ---
int uart_putchar(char c, FILE *stream) {
    if (c == '\n') uart_transmit('\r'); // For terminal compatibility
    uart_transmit(c);
    return 0;
}
