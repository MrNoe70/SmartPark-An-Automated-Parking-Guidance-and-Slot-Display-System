#include "gpio.h"
#include "ultrasonic.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// --- Constants ---
#define DIST_THRESHOLD_CM 10

// --- Finite State Machine (FSM) States ---
typedef enum {
    STATE_NO_CAR,
    STATE_CAR_DETECTED
} ParkingState_t;

// --- Global State Variables ---
volatile ParkingState_t current_state = STATE_NO_CAR;
volatile uint8_t distance_cm = 0;
volatile uint8_t interrupt_flag = 0;

// --- Software interrupt trigger for FSM transition ---
void trigger_interrupt(void) {
    interrupt_flag = 1;
}

int main(void) {
    // --- Hardware Initialization ---
    gpio_set_direction(GPIO_PORT_B, 0, GPIO_PIN_OUTPUT); // LED indicator
    ultrasonic_init();
    uart_init();

    sei(); // Enable global interrupts

    printf("System Initialized...\n");

    while (1) {
        distance_cm = ultrasonic_read_distance();

        // --- FSM: Transition Logic ---
        switch (current_state) {
            case STATE_NO_CAR:
                if (distance_cm < DIST_THRESHOLD_CM) {
                    trigger_interrupt();
                    current_state = STATE_CAR_DETECTED;
                }
                break;

            case STATE_CAR_DETECTED:
                if (distance_cm > DIST_THRESHOLD_CM) {
                    trigger_interrupt();
                    current_state = STATE_NO_CAR;
                }
                break;
        }

        // --- ISR-like Action ---
        if (interrupt_flag) {
            interrupt_flag = 0;

            if (current_state == STATE_CAR_DETECTED) {
                gpio_write(GPIO_PORT_B, 0, GPIO_PIN_HIGH);
                printf("CAR DETECTED - Distance: %d cm\n", distance_cm);
            } else {
                gpio_write(GPIO_PORT_B, 0, GPIO_PIN_LOW);
                printf("NO CAR - Distance: %d cm\n", distance_cm);
            }
        }

        _delay_ms(200); // Delay for stability and readability
    }
}
