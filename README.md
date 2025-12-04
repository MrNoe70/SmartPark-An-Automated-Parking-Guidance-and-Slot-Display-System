# SmartPark: An Automated Parking Guidance and Slot Display System

## Project Overview
SmartPark is a real-time parking management system designed to streamline vehicle parking by providing **real-time slot availability**. Using ultrasonic sensors installed at each parking slot, the system detects whether a space is occupied or free. The collected data is transmitted to a central microcontroller, which updates an LCD display and individual LEDs to guide drivers efficiently.

This solution reduces the time spent searching for parking, minimizes congestion, and enhances user convenience. If all slots are occupied, the system displays **"FULL PARKING"**.

---

## Team Members
- **Noe S Setenta Jr** (noejr.setenta@g.msuit.edu.ph)  
- **Jah Isaac Cagula** (jahisaac.cagula@g.msuiit.edu.ph)

---

## Motivation
Finding available parking in busy areas often leads to wasted time, fuel consumption, and traffic congestion. High-end smart parking systems are costly and not feasible for smaller lots. SmartPark provides a **low-cost, scalable, and reliable alternative** using affordable embedded system components.

---

## System Requirements

### Functional Requirements
- Detect whether each parking slot is occupied or free using sensors.
- Send real-time slot status data to a central microcontroller.
- Update LCD display at the parking entrance to indicate available slots.
- Display **"Parking Full"** when all slots are occupied.
- Update slot availability within **1 second** of a status change.

---

## System Architecture

### Hardware Components
- **MCU:** Arduino Uno R3 (ATmega328P)  
- **Sensors:** 6× HC-SR04 Ultrasonic Sensors  
- **Indicators:** 6× LEDs with resistors  
- **Display:** 16×2 I2C LCD  
- **Power Supply:** 5V DC adapter  
- **Breadboard & Jumper Wires** for assembly  

### Process Flow
1. Initialize LCD, sensors, and LEDs.  
2. Trigger all sensors and wait for echo pulses.  
3. Calculate distance to detect car presence.  
4. Update LEDs and LCD in real-time.  
5. Display **"FULL PARKING"** when all slots are occupied.  

---

## Software / Firmware Implementation

### Code Structure
src/ 
├── main.c          # Main application with FSM 
├── ultrasonic.c/h  # 6-sensor driver with interrupts 
├── gpio.c/h        # GPIO abstraction layer 
├── lcd.c/h         # I2C LCD driver 
└── Makefile        # Build configuration


### Finite State Machine (FSM)
```c
typedef enum {
    STATE_NO_CAR,
    STATE_CAR_DETECTED,
    STATE_ERROR
} ParkingState_t;

NO_CAR → CAR_DETECTED when distance ≤ 10cm

CAR_DETECTED → NO_CAR when distance > 10cm

ERROR_STATE for invalid readings

## Future Enhancements