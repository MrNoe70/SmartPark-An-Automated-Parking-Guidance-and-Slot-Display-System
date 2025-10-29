#include "gpio.h"

// Configure pin direction
void gpio_set_direction(gpio_port_t port, uint8_t pin, gpio_direction_t dir) {
    switch (port) {
        case GPIO_PORT_B:
            if (dir) DDRB |= (1 << pin);
            else DDRB &= ~(1 << pin);
            break;
        case GPIO_PORT_C:
            if (dir) DDRC |= (1 << pin);
            else DDRC &= ~(1 << pin);
            break;
        case GPIO_PORT_D:
            if (dir) DDRD |= (1 << pin);
            else DDRD &= ~(1 << pin);
            break;
    }
}

// Write to pin
void gpio_write(gpio_port_t port, uint8_t pin, gpio_state_t state) {
    switch (port) {
        case GPIO_PORT_B:
            if (state) PORTB |= (1 << pin);
            else PORTB &= ~(1 << pin);
            break;
        case GPIO_PORT_C:
            if (state) PORTC |= (1 << pin);
            else PORTC &= ~(1 << pin);
            break;
        case GPIO_PORT_D:
            if (state) PORTD |= (1 << pin);
            else PORTD &= ~(1 << pin);
            break;
    }
}

// Read pin state
uint8_t gpio_read(gpio_port_t port, uint8_t pin) {
    switch (port) {
        case GPIO_PORT_B: return (PINB & (1 << pin)) != 0;
        case GPIO_PORT_C: return (PINC & (1 << pin)) != 0;
        case GPIO_PORT_D: return (PIND & (1 << pin)) != 0;
    }
    return 0;
}
