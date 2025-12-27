#include "gpio.h"
#include "ultrasonic.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include <util/delay.h>

// Constants
#define DIST_THRESHOLD_CM 10      // Distance threshold for car detection
#define UPDATE_INTERVAL_MS 150    // Time between measurements (150ms)
#define LED_TEST_DELAY_MS  100    // Delay for LED test sequence

// FSM States for Each Slot
typedef enum {
    STATE_NO_CAR,
    STATE_CAR_DETECTED,
    STATE_ERROR
} ParkingState_t;

// Global Variables
ParkingState_t slot_states[NUM_SENSORS] = {STATE_NO_CAR};
uint16_t slot_distances[NUM_SENSORS] = {0};
uint8_t slot_status[NUM_SENSORS] = {0};
uint8_t measurement_cycle = 0;
uint8_t system_ready = 0;
uint8_t lcd_needs_update = 1;

// Function Prototypes
void system_init(void);
void led_test_sequence(void);
uint8_t perform_measurement_cycle(void);
void update_fsm_all(void);
void update_leds(void);
void update_lcd_display(void);
void convert_states_to_status(void);
void display_startup_message(void);
void display_system_status(void);

// Initialize System
void system_init(void) {
    // Initialize LCD first
    lcd_init();
    display_startup_message();
    
    // Initialize LEDs
    led_init();
    
    // Run LED test sequence
    led_test_sequence();
    
    // Initialize ultrasonic sensors
    ultrasonic_init_all();
    
    // Disable SPI to free PB4 (D12) and PB5 (D13)
    SPCR &= ~(1 << SPE);
    
    // Enable global interrupts
    sei();
    
    system_ready = 1;
    
    display_system_status();
    
    _delay_ms(500);
}

// Display Startup Message
void display_startup_message(void) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("SmartPark System");
    lcd_set_cursor(1, 0);
    lcd_print("Initializing...");
    _delay_ms(1500);

    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("By: Noe Setenta");
    lcd_set_cursor(1, 0);
    lcd_print("    Jah Cagula");
    _delay_ms(1500);
}

// Display System Status
void display_system_status(void) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("System Ready");
    lcd_set_cursor(1, 0);
    lcd_print("6 Sensors Active");
    _delay_ms(1000);
}

// LED Test Sequence
void led_test_sequence(void) {
    uint8_t i;
    uint8_t j;
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Testing LEDs...");
    
    // Quick LED test - all at once
    for(j = 0; j < 2; j++) {
        // All LEDs on
        update_sensor_led(SENSOR_1, 5);  // Fake distance to turn on
        update_sensor_led(SENSOR_2, 5);
        update_sensor_led(SENSOR_3, 5);
        update_sensor_led(SENSOR_4, 5);
        update_sensor_led(SENSOR_5, 5);
        update_sensor_led(SENSOR_6, 5);
        _delay_ms(300);
        
        // All LEDs off
        update_sensor_led(SENSOR_1, 20);  // Fake distance to turn off
        update_sensor_led(SENSOR_2, 20);
        update_sensor_led(SENSOR_3, 20);
        update_sensor_led(SENSOR_4, 20);
        update_sensor_led(SENSOR_5, 20);
        update_sensor_led(SENSOR_6, 20);
        _delay_ms(150);
    }
    
    lcd_clear();
}

// Update FSM for a Single Slot
uint8_t update_fsm_slot(SensorID_t sensor_id) {
    uint16_t distance;
    ParkingState_t old_state;
    ParkingState_t new_state;
    uint8_t state_changed = 0;
    
    if(sensor_id >= NUM_SENSORS) return 0;
    
    distance = slot_distances[sensor_id];
    old_state = slot_states[sensor_id];
    new_state = old_state;
    
    if(distance > 0 && distance <= DIST_THRESHOLD_CM) {
        new_state = STATE_CAR_DETECTED;
    } else if(distance == 0) {
        new_state = STATE_ERROR;
    } else {
        new_state = STATE_NO_CAR;
    }
    
    if(old_state != new_state) {
        state_changed = 1;
        slot_states[sensor_id] = new_state;
    }
    
    return state_changed;
}

// Convert States to Status Array
void convert_states_to_status(void) {
    uint8_t i;
    
    for(i = 0; i < NUM_SENSORS; i++) {
        slot_status[i] = (slot_states[i] == STATE_CAR_DETECTED) ? 1 : 0;
    }
}

// Update FSM for All Slots
void update_fsm_all(void) {
    uint8_t i;
    uint8_t any_state_changed = 0;
    
    for(i = 0; i < NUM_SENSORS; i++) {
        if(update_fsm_slot(i)) {
            any_state_changed = 1;
        }
    }
    
    if(any_state_changed) {
        lcd_needs_update = 1;
        convert_states_to_status();
    }
}

// Update LEDs based on current state
void update_leds(void) {
    uint8_t i;
    
    for(i = 0; i < NUM_SENSORS; i++) {
        update_sensor_led(i, slot_distances[i]);
    }
}

// Update LCD Display
void update_lcd_display(void) {
    uint8_t i;
    uint8_t occupied_count = 0;
    
    for(i = 0; i < NUM_SENSORS; i++) {
        if(slot_status[i] == 1) {
            occupied_count++;
        }
    }
    
    if(occupied_count == NUM_SENSORS) {
        lcd_clear();
        lcd_set_cursor(0, 2);
        lcd_print("FULL PARKING");
        lcd_set_cursor(1, 1);
        lcd_print("NO SPACES");
    } else {
        lcd_clear();
        
        // Line 1: P0:0 P1:0 P2:0
        lcd_set_cursor(0, 0);
        lcd_print("P0:");
        lcd_data('0' + slot_status[0]);
        lcd_print(" P1:");
        lcd_data('0' + slot_status[1]);
        lcd_print(" P2:");
        lcd_data('0' + slot_status[2]);
        
        // Line 2: P3:0 P4:0 P5:0
        lcd_set_cursor(1, 0);
        lcd_print("P3:");
        lcd_data('0' + slot_status[3]);
        lcd_print(" P4:");
        lcd_data('0' + slot_status[4]);
        lcd_print(" P5:");
        lcd_data('0' + slot_status[5]);
    }
}

// Perform Measurement Cycle
uint8_t perform_measurement_cycle(void) {
    uint8_t all_valid = 1;
    uint8_t i;
    uint16_t timeout = 0;
    uint8_t all_done = 0;
    
    ultrasonic_trigger_all();
    
    while(!all_done && timeout < 10000) {
        all_done = 1;
        for(i = 0; i < NUM_SENSORS; i++) {
            if(!ultrasonic_is_measurement_done(i)) {
                all_done = 0;
                break;
            }
        }
        timeout++;
        _delay_us(1);
    }
    
    for(i = 0; i < NUM_SENSORS; i++) {
        slot_distances[i] = ultrasonic_get_distance(i);
        
        if(slot_distances[i] == 0) {
            all_valid = 0;
        }
        
        ultrasonic_reset_measurement(i);
    }
    
    return all_valid;
}

// Main Application
int main(void) {
    uint8_t measurements_valid;
    
    system_init();
    
    convert_states_to_status();
    update_lcd_display();
    
    while(1) {
        measurements_valid = perform_measurement_cycle();
        
        update_fsm_all();
        
        update_leds();
        
        if(lcd_needs_update) {
            update_lcd_display();
            lcd_needs_update = 0;
        }
        
        // Force LCD update every 10 seconds (safety measure)
        measurement_cycle++;
        if(measurement_cycle >= 67) { // 67 cycles * 150ms = ~10 seconds
            measurement_cycle = 0;
            convert_states_to_status();
            update_lcd_display();
        }
        
        _delay_ms(UPDATE_INTERVAL_MS);
    }
    
    return 0;
}