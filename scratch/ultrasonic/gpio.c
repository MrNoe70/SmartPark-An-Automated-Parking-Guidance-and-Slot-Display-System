#include "gpio.h"

// Helper arrays to map enum to actual registers
static volatile uint8_t* ddr_registers[] = {&DDRB, &DDRC, &DDRD};
static volatile uint8_t* port_registers[] = {&PORTB, &PORTC, &PORTD};
static volatile uint8_t* pin_registers[] = {&PINB, &PINC, &PIND};

void gpio_set_direction(GpioPort_t port, uint8_t pin_num, GpioDirection_t direction) {
    if (direction == GPIO_PIN_OUTPUT) {
        *ddr_registers[port] |= (1 << pin_num); // Set bit for Output
    } else {
        *ddr_registers[port] &= ~(1 << pin_num); // Clear bit for Input
    }
}   

void gpio_write(GpioPort_t port, uint8_t pin_num, GpioValue_t value) {
        if (value == GPIO_PIN_HIGH) {
        *port_registers[port] |= (1 << pin_num); // Set bit to HIGH
    } else {
        *port_registers[port] &= ~(1 << pin_num); // Set bit to LOW
    }
}

void gpio_set_pullup(GpioPort_t port, uint8_t pin_num, uint8_t enable) {
    // Enabling a pull-up is the same as writing HIGH to an input pin.
    // Portx register controls pull-ups when pin in Input
    if (enable) {
        *port_registers[port] |= (1 << pin_num); // Enable pull-up
    } else {
        *port_registers[port] &= ~(1 << pin_num); // Disable pull-up
    }   
}

GpioValue_t gpio_read(GpioPort_t port, uint8_t pin_num) {
    // PINx register contains the actual pin state (input values)
    if ((*pin_registers[port]) & (1 << pin_num)) {
        return GPIO_PIN_HIGH;
    } else {
        return GPIO_PIN_LOW;
    }
}