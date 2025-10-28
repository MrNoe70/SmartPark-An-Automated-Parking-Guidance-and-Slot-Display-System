#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdio.h>

// --- UART Configuration ---
#define BAUD_RATE 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD_RATE)) - 1)

// --- Function Prototypes ---
void uart_init(void);
void uart_transmit(char data);
void uart_print(const char* str);
int uart_putchar(char c, FILE *stream);

#endif // UART_H
