#include "ultrasonic.h"
#include "gpio.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// --- Module-Level Variables ---
volatile uint16_t pulse_start[NUM_SENSORS] = {0};
volatile uint16_t pulse_end[NUM_SENSORS] = {0};
volatile uint8_t measurement_active[NUM_SENSORS] = {0};
volatile uint8_t measurement_done[NUM_SENSORS] = {0};
volatile uint8_t last_portb_state = 0;

// --- Helper Functions ---
static uint8_t get_trigger_pin(SensorID_t sensor_id) {
    switch(sensor_id) {
        case SENSOR_1: return TRIGGER_1_PIN;
        case SENSOR_2: return TRIGGER_2_PIN;
        case SENSOR_3: return TRIGGER_3_PIN;
        case SENSOR_4: return TRIGGER_4_PIN;
        case SENSOR_5: return TRIGGER_5_PIN;
        case SENSOR_6: return TRIGGER_6_PIN;
        default: return TRIGGER_1_PIN;
    }
}

// --- Initialize All Ultrasonic Sensors ---
void ultrasonic_init_all(void) {
    uint8_t i;
    
    // Initialize all trigger pins as outputs (LOW)
    for(i = 0; i < NUM_SENSORS; i++) {
        uint8_t pin = get_trigger_pin(i);
        gpio_set_direction(TRIGGER_PORT, pin, GPIO_PIN_OUTPUT);
        gpio_write(TRIGGER_PORT, pin, GPIO_PIN_LOW);
    }
    
    // Initialize echo pins as inputs
    gpio_set_direction(ECHO_1_PORT, ECHO_1_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(ECHO_2_PORT, ECHO_2_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(ECHO_3_PORT, ECHO_3_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(ECHO_4_PORT, ECHO_4_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(ECHO_5_PORT, ECHO_5_PIN, GPIO_PIN_INPUT);
    gpio_set_direction(ECHO_6_PORT, ECHO_6_PIN, GPIO_PIN_INPUT);
    
    // Disable pull-ups on all echo pins
    gpio_set_pullup(ECHO_1_PORT, ECHO_1_PIN, 0);
    gpio_set_pullup(ECHO_2_PORT, ECHO_2_PIN, 0);
    gpio_set_pullup(ECHO_3_PORT, ECHO_3_PIN, 0);
    gpio_set_pullup(ECHO_4_PORT, ECHO_4_PIN, 0);
    gpio_set_pullup(ECHO_5_PORT, ECHO_5_PIN, 0);
    gpio_set_pullup(ECHO_6_PORT, ECHO_6_PIN, 0);
    
    // Configure Timer1 for microsecond timing (prescaler 8)
    TCCR1A = 0;
    TCCR1B = (1 << CS11); // Prescaler = 8 (0.5µs per tick at 16MHz)
    
    // Configure Pin Change Interrupts for all sensors (PORTB)
    PCICR |= (1 << PCIE0);                  // Enable PCINT0_vect (PORTB)
    PCMSK0 |= (1 << PCINT0) |               // PB0 (Sensor 1)
              (1 << PCINT1) |               // PB1 (Sensor 2)
              (1 << PCINT2) |               // PB2 (Sensor 3)
              (1 << PCINT3) |               // PB3 (Sensor 4)
              (1 << PCINT4) |               // PB4 (Sensor 5) - D12
              (1 << PCINT5);                // PB5 (Sensor 6) - D13
    
    // Store initial state of PORTB for change detection
    last_portb_state = PINB;
    
    // Reset all measurements
    for(i = 0; i < NUM_SENSORS; i++) {
        ultrasonic_reset_measurement(i);
    }
}

// --- Initialize LEDs ---
void led_init(void) {
    // Initialize LED pins as outputs (different ports)
    
    // Sensor 1-4 LEDs on PORTC (A0-A3)
    gpio_set_direction(LED1_PORT, LED1_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(LED2_PORT, LED2_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(LED3_PORT, LED3_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(LED4_PORT, LED4_PIN, GPIO_PIN_OUTPUT);
    
    // Sensor 5-6 LEDs on PORTD (D0-D1)
    gpio_set_direction(LED5_PORT, LED5_PIN, GPIO_PIN_OUTPUT);
    gpio_set_direction(LED6_PORT, LED6_PIN, GPIO_PIN_OUTPUT);
    
    // Turn all LEDs off initially
    gpio_write(LED1_PORT, LED1_PIN, GPIO_PIN_LOW);
    gpio_write(LED2_PORT, LED2_PIN, GPIO_PIN_LOW);
    gpio_write(LED3_PORT, LED3_PIN, GPIO_PIN_LOW);
    gpio_write(LED4_PORT, LED4_PIN, GPIO_PIN_LOW);
    gpio_write(LED5_PORT, LED5_PIN, GPIO_PIN_LOW);
    gpio_write(LED6_PORT, LED6_PIN, GPIO_PIN_LOW);
}

// --- Update LED for a specific sensor ---
void update_sensor_led(SensorID_t sensor_id, uint16_t distance) {
    if(sensor_id >= NUM_SENSORS) return;
    
    // Determine LED state based on distance
    GpioValue_t led_state = (distance > 0 && distance <= 10) ? 
                            GPIO_PIN_HIGH : GPIO_PIN_LOW;
    
    // Update the appropriate LED
    switch(sensor_id) {
        case SENSOR_1:
            gpio_write(LED1_PORT, LED1_PIN, led_state);
            break;
        case SENSOR_2:
            gpio_write(LED2_PORT, LED2_PIN, led_state);
            break;
        case SENSOR_3:
            gpio_write(LED3_PORT, LED3_PIN, led_state);
            break;
        case SENSOR_4:
            gpio_write(LED4_PORT, LED4_PIN, led_state);
            break;
        case SENSOR_5:
            gpio_write(LED5_PORT, LED5_PIN, led_state);
            break;
        case SENSOR_6:
            gpio_write(LED6_PORT, LED6_PIN, led_state);
            break;
    }
}

// --- Update all LEDs based on distances ---
void update_all_leds(uint16_t distances[]) {
    uint8_t i;
    
    for(i = 0; i < NUM_SENSORS; i++) {
        update_sensor_led(i, distances[i]);
    }
}

// --- Trigger All Sensors Simultaneously ---
void ultrasonic_trigger_all(void) {
    uint8_t i;
    
    // Reset measurement flags
    for(i = 0; i < NUM_SENSORS; i++) {
        measurement_done[i] = 0;
        measurement_active[i] = 0;
    }
    
    // Send 10µs pulse to all trigger pins
    for(i = 0; i < NUM_SENSORS; i++) {
        uint8_t pin = get_trigger_pin(i);
        gpio_write(TRIGGER_PORT, pin, GPIO_PIN_HIGH);
    }
    _delay_us(10);
    for(i = 0; i < NUM_SENSORS; i++) {
        uint8_t pin = get_trigger_pin(i);
        gpio_write(TRIGGER_PORT, pin, GPIO_PIN_LOW);
    }
}

// --- Trigger Single Sensor ---
void ultrasonic_trigger_single(SensorID_t sensor_id) {
    if(sensor_id >= NUM_SENSORS) return;
    
    measurement_done[sensor_id] = 0;
    measurement_active[sensor_id] = 0;
    
    uint8_t pin = get_trigger_pin(sensor_id);
    gpio_write(TRIGGER_PORT, pin, GPIO_PIN_HIGH);
    _delay_us(10);
    gpio_write(TRIGGER_PORT, pin, GPIO_PIN_LOW);
}

// --- Get Distance from Specific Sensor ---
uint16_t ultrasonic_get_distance(SensorID_t sensor_id) {
    uint32_t pulse_duration;
    uint32_t duration_us;
    uint16_t distance_cm;
    
    if(sensor_id >= NUM_SENSORS) return 0;
    if(!measurement_done[sensor_id]) return 0;
    
    // Calculate pulse duration
    if(pulse_end[sensor_id] >= pulse_start[sensor_id]) {
        pulse_duration = pulse_end[sensor_id] - pulse_start[sensor_id];
    } else {
        // Handle timer overflow
        pulse_duration = (65535 - pulse_start[sensor_id]) + pulse_end[sensor_id];
    }
    
    // Convert timer ticks to microseconds (0.5µs per tick)
    duration_us = pulse_duration / 2;
    
    // Convert to centimeters (58µs per cm round trip)
    distance_cm = duration_us / 58;
    
    // Validate distance range
    if(distance_cm < MIN_DISTANCE_CM || distance_cm > MAX_DISTANCE_CM) {
        return 0;
    }
    
    return distance_cm;
}

// --- Check if Measurement is Complete ---
uint8_t ultrasonic_is_measurement_done(SensorID_t sensor_id) {
    if(sensor_id >= NUM_SENSORS) return 0;
    return measurement_done[sensor_id];
}

// --- Reset Measurement for a Sensor ---
void ultrasonic_reset_measurement(SensorID_t sensor_id) {
    if(sensor_id >= NUM_SENSORS) return;
    measurement_done[sensor_id] = 0;
    measurement_active[sensor_id] = 0;
    pulse_start[sensor_id] = 0;
    pulse_end[sensor_id] = 0;
}

// --- Pin Change Interrupt Service Routine for PORTB (All 6 sensors) ---
ISR(PCINT0_vect) {
    uint8_t current_state = PINB;
    uint8_t changed_bits = current_state ^ last_portb_state;
    
    // Sensor 1 (PB0/D8)
    if(changed_bits & (1 << PB0)) {
        if(current_state & (1 << PB0)) {
            pulse_start[SENSOR_1] = TCNT1;
            measurement_active[SENSOR_1] = 1;
        } else {
            if(measurement_active[SENSOR_1]) {
                pulse_end[SENSOR_1] = TCNT1;
                measurement_done[SENSOR_1] = 1;
                measurement_active[SENSOR_1] = 0;
            }
        }
    }
    
    // Sensor 2 (PB1/D9)
    if(changed_bits & (1 << PB1)) {
        if(current_state & (1 << PB1)) {
            pulse_start[SENSOR_2] = TCNT1;
            measurement_active[SENSOR_2] = 1;
        } else {
            if(measurement_active[SENSOR_2]) {
                pulse_end[SENSOR_2] = TCNT1;
                measurement_done[SENSOR_2] = 1;
                measurement_active[SENSOR_2] = 0;
            }
        }
    }
    
    // Sensor 3 (PB2/D10)
    if(changed_bits & (1 << PB2)) {
        if(current_state & (1 << PB2)) {
            pulse_start[SENSOR_3] = TCNT1;
            measurement_active[SENSOR_3] = 1;
        } else {
            if(measurement_active[SENSOR_3]) {
                pulse_end[SENSOR_3] = TCNT1;
                measurement_done[SENSOR_3] = 1;
                measurement_active[SENSOR_3] = 0;
            }
        }
    }
    
    // Sensor 4 (PB3/D11)
    if(changed_bits & (1 << PB3)) {
        if(current_state & (1 << PB3)) {
            pulse_start[SENSOR_4] = TCNT1;
            measurement_active[SENSOR_4] = 1;
        } else {
            if(measurement_active[SENSOR_4]) {
                pulse_end[SENSOR_4] = TCNT1;
                measurement_done[SENSOR_4] = 1;
                measurement_active[SENSOR_4] = 0;
            }
        }
    }
    
    // Sensor 5 (PB4/D12)
    if(changed_bits & (1 << PB4)) {
        if(current_state & (1 << PB4)) {
            pulse_start[SENSOR_5] = TCNT1;
            measurement_active[SENSOR_5] = 1;
        } else {
            if(measurement_active[SENSOR_5]) {
                pulse_end[SENSOR_5] = TCNT1;
                measurement_done[SENSOR_5] = 1;
                measurement_active[SENSOR_5] = 0;
            }
        }
    }
    
    // Sensor 6 (PB5/D13)
    if(changed_bits & (1 << PB5)) {
        if(current_state & (1 << PB5)) {
            pulse_start[SENSOR_6] = TCNT1;
            measurement_active[SENSOR_6] = 1;
        } else {
            if(measurement_active[SENSOR_6]) {
                pulse_end[SENSOR_6] = TCNT1;
                measurement_done[SENSOR_6] = 1;
                measurement_active[SENSOR_6] = 0;
            }
        }
    }
    
    last_portb_state = current_state;
}