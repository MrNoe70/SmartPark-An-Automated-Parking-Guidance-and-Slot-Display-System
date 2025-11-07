# SmartPark: An Automated Parking Guidance and Slot Display System

## 📘 Project Overview
**SmartPark** is an embedded system project designed to streamline vehicle parking by providing **real-time information** on slot availability. It uses **ultrasonic sensors** to detect whether parking spaces are occupied or free, sending the data to a **central controller** that updates an **LCD display** at the parking entrance.

This allows drivers to quickly locate available parking spaces, reducing time, congestion, and fuel consumption. The system is designed to be **low-cost, scalable, and suitable for small to medium-sized parking facilities**.

---

## 👨‍💻 Team Members
- **Noe S Setenta Jr**
- **Jah Isaac Cagula**

---

## 🧩 Features
- Real-time detection of parking slot occupancy.
- Centralized control via **Arduino Uno**.
- LCD display showing available parking slots.
- Automatic "Parking Full" indicator when all slots are occupied.
- Update latency under **2 seconds**.
- Modular and scalable system design.

---

## ⚙️ System Requirements

### Functional Requirements
1. Detects whether each parking slot is occupied or free using sensors.  
2. Sends real-time slot status data to a central microcontroller.  
3. Updates an LCD display at the parking entrance to indicate available slots.  
4. Displays **“Parking Full”** when all slots are occupied.  
5. Reflects changes in slot status within **2 seconds**.

### Non-Functional Requirements
- **Performance:** Display updates within < 2 seconds after sensor change.  
- **Power:** Operates on a standard 5V DC supply.  
- **Reliability:** Accurate detection without false readings.  
- **Usability:** Clear, easy-to-read LCD messages visible from a distance.  
- **Maintainability:** Easy to replace or expand sensors.  
- **Scalability:** Supports future expansion to more slots or multiple entrances.  
- **Durability:** Resistant to environmental factors (dust, humidity, temperature).

---

## 🧠 System Design
The system consists of:
- **Sensor Nodes:** Ultrasonic sensors (HC-SR04) for each parking slot.
- **Controller Unit:** Arduino Uno processes the sensor data.
- **Display Module:** 16x2 I2C LCD showing the number of available slots or “Parking Full”.
- **Power Source:** 5V DC wall adapter.

---

## 🧾 Bill of Materials (BOM)

| Component | Quantity | Estimated Cost | Notes |
|------------|-----------|----------------|-------|
| Arduino Uno (MCU) | 1 | — | Main controller |
| Ultrasonic Sensor (HC-SR04) | 6 | ₱300 | Detects slot occupancy |
| 16x2 I2C LCD Display | 1 | — | Displays slot status |
| Breadboard | 1 | — | Circuit assembly |
| Jumper Wires | 1 set | — | Connections |
| Power Supply | 1 | — | System power |

---

## 🧪 Expected Outcome
The expected output is a **fully functional prototype** that can:
- Accurately display real-time parking slot availability.
- Reduce driver frustration and traffic congestion.
- Demonstrate an affordable and scalable smart parking solution.

---

## 🧭 Future Improvements
- Add wireless data transmission (e.g., Wi-Fi or LoRa) for remote monitoring.  
- Integrate a mobile or web interface for real-time slot viewing.  
- Implement power-saving features for outdoor or solar-powered deployment.

---

## 🧰 Tools & Technologies
- **Arduino IDE** (Bare-Metal C/C++ programming)
- **Ultrasonic Sensors (HC-SR04)**
- **I2C LCD Display (16x2)**
- **Embedded Systems Design Principles**

---

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).

---

## 📫 Contact
For inquiries or collaboration:
- **Email:** [noejr.setenta@g.msuiit.edu.ph]  
- **GitHub:** [@noesetenta](https://github.com/MrNoe70)

---

### 🚗 SmartPark — Making Parking Smarter, One Slot at a Time.
