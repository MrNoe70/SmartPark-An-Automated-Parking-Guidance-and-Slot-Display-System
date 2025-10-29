#include "ultrasonic.h"
#include "gpio.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// --- Module-Level Variables ---
// These are volatile because they are modified inside an ISR.
volatile uint16_t timer_count = 0;
volatile uint8_t echo_rising_edge = 0;
volatile uint8_t echo_done = 0;

// --- Interrupt Service Routine for Echo Pin (INT1) ---
// Captures time-of-flight by timing HIGH pulse duration on ECHO line.
ISR(INT1_vect) {
    if (!echo_rising_edge) {
        // Rising edge detected: Start timing
        TCNT1 = 0;                  // Reset timer
        echo_rising_edge = 1;
    } else {
        // Falling edge detected: Stop timing
        timer_count = TCNT1;
        echo_done = 1;
        echo_rising_edge = 0;
    }
}

// --- Initialize Ultrasonic Sensor Interface ---
void ultrasonic_init(void) {
    // Configure Trigger pin as output, Echo pin as input
    gpio_set_direction(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(GPIO_PORT_D, ULTRASONIC_ECHO_PIN, GPIO_PIN_INPUT);

    // Enable pull-up on Echo for stability (optional)
    gpio_set_pullup(GPIO_PORT_D, ULTRASONIC_ECHO_PIN, 0);

    // Configure Timer1 for precise pulse timing
    TCCR1A = 0;                     // Normal mode
    TCCR1B = (1 << CS11);           // Prescaler = 8 (0.5µs per tick at 16MHz)

    // Enable external interrupt on INT1 (PD3)
    EICRA |= (1 << ISC10);          // Trigger on any logical change
    EIMSK |= (1 << INT1);           // Enable INT1 interrupt
}

// --- Measure Distance in Centimeters ---
// Sends a 10µs trigger pulse, waits for echo, and computes distance.
uint16_t ultrasonic_read_distance(void) {
    echo_done = 0;

    // Trigger a 10µs pulse to start measurement
    gpio_write(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, GPIO_PIN_HIGH);
    _delay_us(10);
    gpio_write(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, GPIO_PIN_LOW);

    // Wait for measurement completion (ISR sets echo_done)
    while (!echo_done);

    // Convert timer ticks to distance:
    // Sound speed = 343 m/s => 58 µs per cm (round trip)
    // Tick time = 0.5 µs (prescaler 8)
    uint32_t duration_us = timer_count / 2;
    uint16_t distance_cm = duration_us / 58;

    return distance_cm;
}
