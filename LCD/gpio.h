#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>
#include <stdint.h>

// Logical pin states
typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_state_t;

// Direction control
typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
} gpio_direction_t;

// Port abstraction
typedef enum {
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D
} gpio_port_t;

// Function prototypes
void gpio_set_direction(gpio_port_t port, uint8_t pin, gpio_direction_t dir);
void gpio_write(gpio_port_t port, uint8_t pin, gpio_state_t state);
uint8_t gpio_read(gpio_port_t port, uint8_t pin);
void gpio_toggle(gpio_port_t port, uint8_t pin);

#endif
