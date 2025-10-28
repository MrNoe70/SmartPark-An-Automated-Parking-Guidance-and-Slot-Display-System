#include "gpio.h"
#include "ultrasonic.h"
#include <avr/interrupt.h>
#include <util/delay.h>

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

// --- Simulated Software Interrupt Trigger ---
// (Used to decouple state logic from hardware response)
void trigger_interrupt(void) {
    interrupt_flag = 1;
}

int main(void) {
    // --- Hardware Initialization ---
    gpio_set_direction(GPIO_PORT_B, 0, GPIO_PIN_OUTPUT); // LED for indicator
    ultrasonic_init();

    sei(); // Enable global interrupts

    while (1) {
        distance_cm = ultrasonic_read_distance();

        // --- FSM: State Transition Logic ---
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

        // --- ISR-like Response to State Change ---
        if (interrupt_flag) {
            interrupt_flag = 0;
            if (current_state == STATE_CAR_DETECTED) {
                gpio_write(GPIO_PORT_B, 0, GPIO_PIN_HIGH); // LED ON
            } else {
                gpio_write(GPIO_PORT_B, 0, GPIO_PIN_LOW);  // LED OFF
            }
        }

        _delay_ms(100); // Sampling delay to reduce sensor jitter
    }
}
