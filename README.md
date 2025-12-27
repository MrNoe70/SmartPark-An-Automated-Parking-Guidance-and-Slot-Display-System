# SmartPark: An Automated Parking Guidance and Slot Display System

## Project Overview
SmartPark is a real-time parking management system designed to streamline vehicle parking by providing **real-time slot availability**. Using ultrasonic sensors installed at each parking slot, the system detects whether a space is occupied or free. The collected data is transmitted to a central microcontroller, which updates an LCD display and individual LEDs to guide drivers efficiently.

This solution reduces the time spent searching for parking, minimizes congestion, and enhances user convenience. If all slots are occupied, the system displays **"FULL PARKING"**.

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

## Setup Instructions   

### Hardware Setup
1. Connect HC-SR04 sensors:
    - Sensor 1: Trig=D2, Echo=D8, LED=A0
    - Sensor 2: Trig=D3, Echo=D9, LED=A1
    - Sensor 3: Trig=D4, Echo=D10, LED=A2
    - Sensor 4: Trig=D5, Echo=D11, LED=A3
    - Sensor 5: Trig=D6, Echo=D12, LED=D0
    - Sensor 6: Trig=D7, Echo=D13, LED=D1

2. Connect LCD (I2C 16x2):
    - SDA=A4, SCL=A5    

3. Power all components with 5V DC supply.

4. Place LEDs with 2000Ω resistors in series.

## Software Setup
1. Install Arduino IDE.

2. Clone the repository: https://github.com/MrNoe70/SmartPark-An-Automated-Parking-Guidance-and-Slot-Display-System.git

3. Open the project in Arduino IDE.

4. Upload the firmware to the Arduino Uno.

5. Observe LCD and LED indicators for real-time slot updates.

---

## Prototype (To be added)
1. Front view showing sensors and LCD

2. Wiring close-up

3. LED indicators in operation

4. Full system test setup

---

## Future Enhancements
- Wireless status reporting via Bluetooth
- Mobile app interface
- Parking history logging
- Solar-powered capability
- Weather-resistant enclosure

---

## License
This project is developed for academic purposes under the College of Engineering, MSU-IIT. Feel free to use and adapt with proper attribution.
