# 🚗 Vehicle Accident Alert System (Arduino)

An Arduino-based **Vehicle Accident Alert System** designed to detect severe vehicle crashes and automatically send emergency alerts along with the accident location.

This system can operate **without a smartphone**, using onboard communication modules to ensure quick response during emergencies.

---

## 🔍 Features
- Detects vehicle accidents using an accelerometer
- Automatically sends alert messages during a crash
- Shares real-time location of the accident
- Works independently without mobile phone support
- Compact and suitable for vehicle installation

---

## 🧰 Components Used
- Arduino Nano
- ADXL335 Accelerometer
- SIM800L GSM Module
- NEO-6M GPS Module
- Buck Converter (12V to 5V)
- Buzzer / Speaker
- Connecting wires
- Power supply

---

## 🔌 Circuit Connections

### 📍 ADXL335 (Accelerometer)

| ADXL335 Pin | Connected To | Description |
|-------------|-------------|-------------|
| VCC         | +5V (Buck Converter Vo+) | Power Supply |
| X_OUT       | A1 (Arduino Nano) | X-axis Analog Output |
| Y_OUT       | A2 (Arduino Nano) | Y-axis Analog Output |
| Z_OUT       | A3 (Arduino Nano) | Z-axis Analog Output |
| GND         | GND (Buck Converter Vo-) | Ground |

---

### 📍 GPS Neo-6M

| GPS Pin | Connected To | Description |
|----------|-------------|-------------|
| VCC      | +5V (Buck Converter Vo+) | Power Supply |
| RX       | D9 (Arduino Nano) | Receives Data from Arduino |
| TX       | D10 (Arduino Nano) | Sends Data to Arduino |
| GND      | GND (Buck Converter Vo-) | Ground |

---

### 📍 SIM800L GSM Module

| SIM800L Pin | Connected To | Description |
|-------------|-------------|-------------|
| MICp        | Microphone +ve | Audio Input |
| MICn        | Microphone -ve | Audio Input |
| SPKp        | Speaker +ve | Audio Output |
| SPKn        | Speaker -ve | Audio Output |
| VCC         | +5V (Buck Converter Vo+) | Power Supply |
| RSD         | D3 (Arduino Nano) | SoftwareSerial RX |
| TXD         | D2 (Arduino Nano) | SoftwareSerial TX |
| GND         | GND (Buck Converter Vo-) | Ground |

---

### 📍 Arduino Nano

| Arduino Pin | Connected To | Description |
|-------------|-------------|-------------|
| Vin         | +5V (Buck Converter Vo+) | Main Power Input |
| GND         | GND (Buck Converter Vo-) | Ground |

---

### 📍 Buck Converter

| Buck Converter Pin | Connected To | Description |
|--------------------|-------------|-------------|
| Vin+               | +12V | Input from Vehicle Battery |
| Vin-               | GND | Ground |
| Vo+                | +5V | Regulated Output to System |
| Vo-                | GND | Common Ground |

---

## ⚙️ Working Principle
1. The accelerometer continuously monitors sudden changes in motion.
2. If the acceleration exceeds a predefined threshold, it is considered an accident.
3. The system triggers an alert.
4. GPS module fetches the current location.
5. GSM module sends an SMS or call to the predefined emergency number with the location details.

---

## 🚀 Applications
- Cars and commercial vehicles
- Fleet safety systems
- Emergency response systems
- Smart transportation projects

---

## 📌 Future Improvements
- Mobile app integration
- IoT cloud dashboard
- Crash severity classification
- Automatic emergency service call

---

## 👤 Author
**Sahad Nisham. K**  
Electronics & Communication Engineering  
Project: Vehicle Accident Alert System  
Instagram: https://www.instagram.com/sahadnisham.k/  
LinkedIn : https://www.linkedin.com/in/sahad-nisham-k/

---

## 📜 License
This project is open-source and available for educational use.
