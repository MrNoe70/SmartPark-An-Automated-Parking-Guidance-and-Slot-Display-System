#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <stdint.h>

// Number of Sensors 
#define NUM_SENSORS 6

// Sensor IDs 
typedef enum {
    SENSOR_1,
    SENSOR_2, 
    SENSOR_3,
    SENSOR_4,
    SENSOR_5,
    SENSOR_6
} SensorID_t;

// Pin Configuration for 6 Sensors 
// TRIGGER pins (all outputs on PORTD)
#define TRIGGER_PORT   GPIO_PORT_D
#define TRIGGER_1_PIN  2  // PD2 
#define TRIGGER_2_PIN  3  // PD3 
#define TRIGGER_3_PIN  4  // PD4 
#define TRIGGER_4_PIN  5  // PD5 
#define TRIGGER_5_PIN  6  // PD6 
#define TRIGGER_6_PIN  7  // PD7 

// ECHO pins (inputs with interrupts)
#define ECHO_1_PORT    GPIO_PORT_B
#define ECHO_1_PIN     0  // PB0 - PCINT0
#define ECHO_2_PORT    GPIO_PORT_B  
#define ECHO_2_PIN     1  // PB1 - PCINT1
#define ECHO_3_PORT    GPIO_PORT_B
#define ECHO_3_PIN     2  // PB2 - PCINT2
#define ECHO_4_PORT    GPIO_PORT_B
#define ECHO_4_PIN     3  // PB3 - PCINT3
#define ECHO_5_PORT    GPIO_PORT_B
#define ECHO_5_PIN     4  // PB4 - PCINT4
#define ECHO_6_PORT    GPIO_PORT_B
#define ECHO_6_PIN     5  // PB5 - PCINT5

// LED Configuration
// Sensors 1-4 LEDs on PORTC, Sensors 5-6 LEDs on PORTD
#define LED1_PORT      GPIO_PORT_C
#define LED1_PIN       0  // PC0 - Sensor 1 LED
#define LED2_PORT      GPIO_PORT_C
#define LED2_PIN       1  // PC1 - Sensor 2 LED
#define LED3_PORT      GPIO_PORT_C
#define LED3_PIN       2  // PC2 - Sensor 3 LED
#define LED4_PORT      GPIO_PORT_C
#define LED4_PIN       3  // PC3 - Sensor 4 LED
#define LED5_PORT      GPIO_PORT_D
#define LED5_PIN       0  // PD0 - Sensor 5 LED
#define LED6_PORT      GPIO_PORT_D
#define LED6_PIN       1  // PD1 - Sensor 6 LED

// Timing Constants 
#define PULSE_TIMEOUT_US   30000  // 30ms timeout
#define MIN_DISTANCE_CM    2      // Minimum reliable distance
#define MAX_DISTANCE_CM    400    // Maximum reliable distance

// Public API Prototypes 
void ultrasonic_init_all(void);
void led_init(void);
void ultrasonic_trigger_all(void);
void ultrasonic_trigger_single(SensorID_t sensor_id);
uint16_t ultrasonic_get_distance(SensorID_t sensor_id);
uint8_t ultrasonic_is_measurement_done(SensorID_t sensor_id);
void ultrasonic_reset_measurement(SensorID_t sensor_id);
void update_sensor_led(SensorID_t sensor_id, uint16_t distance);
void update_all_leds(uint16_t distances[]);

#endif // ULTRASONIC_H