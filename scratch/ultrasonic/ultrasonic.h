#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <stdint.h>

// --- Hardware Pin Configuration ---
// These macros allow flexible pin mapping and easy migration.
#define ULTRASONIC_TRIGGER_PORT GPIO_PORT_D
#define ULTRASONIC_TRIGGER_PIN  2
#define ULTRASONIC_ECHO_PIN     3  // Uses INT1 external interrupt (PD3)

// --- Public API Prototypes ---
void ultrasonic_init(void);
uint16_t ultrasonic_read_distance(void);

#endif // ULTRASONIC_H
