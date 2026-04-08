# BioHelm: Smart Safety Helmet with Integrated Biosensors

An innovative helmet with integrated biosensors for real-time health monitoring in high-risk environments. Inspired by Formula 1 biometric monitoring systems, BioHelm continuously tracks vital signs and detects emergencies wirelessly.

## Overview

BioHelm is a wearable biometric monitoring system embedded into a safety helmet. It continuously measures heart rate, blood oxygen levels, stress indicators, and motion/impact detection. Data streams wirelessly via Bluetooth Low Energy (BLE) to smartphones and tablets for remote monitoring by supervisors and emergency responders.

Instead of waiting for someone to report an injury, BioHelm proactively monitors health status and alerts when critical conditions occur.

## Key Features

- **Real-Time Vital Sign Monitoring**
  - Heart Rate (BPM): 40-200 range
  - Blood Oxygen Saturation (SpO2): 80-100%
  - Stress Level: 0-100% (via skin conductance)
  - Motion & Impact Detection: Acceleration-based

- **Wireless Data Streaming**
  - BLE connectivity (10+ meter range)
  - Compatible with Light Blue app and standard BLE devices
  - Updates every 1 second
  - Real-time notifications on critical alerts

- **On-Device Display**
  - 128×64 OLED screen shows vital signs
  - Cycles through all metrics
  - Status indicators for normal/critical conditions

- **Safety Intelligence**
  - Automatic critical condition detection
  - Impact/crash detection via accelerometer
  - Color-coded alerts (green/yellow/red)
  - Responsive crash detection (checks every 100ms)

- **Low Power Design**
  - Battery-powered operation
  - Efficient sensor sampling
  - Optimized BLE advertising

## Hardware Components

### Sensors
| Component | Model | Purpose | Connection |
|-----------|-------|---------|-----------|
| Heart Rate & SpO2 | MAX30102 | Optical pulse oximetry | I2C (0x57) |
| Motion Detection | MMA8451 | 3-axis accelerometer | I2C (0x1D) |
| Stress Monitoring | GSR Electrodes | Skin conductance | Analog (GPIO 34) |
| Display | SSD1306 OLED | 128×64 vital signs display | I2C (0x3C) |

### Controllers & Power
| Component | Specs | Purpose |
|-----------|-------|---------|
| ESP32 DevKit | Dual-core, 240MHz | Main processor + WiFi/BLE |
| Battery | 3.7V LiPo | Portable power |
| Charger Module | TP4056-style | Safe battery charging |
| LEDs | RGB | Visual alert indicators |

### Additional
- Breadboard (prototyping)
- Resistors (LED current limiting)
- Jumper wires (connections)
- Custom 3D-printed enclosure

## Real-World Applications

### Construction Sites
- Worker safety monitoring
- Fall detection and injury assessment
- Fatigue and stress level tracking
- Remote supervisor awareness

### Emergency Response
- Firefighter health monitoring
- Heat stress detection
- Exertion level tracking
- Incident response data collection

### Extreme Sports
- Altitude sickness detection (SpO2 monitoring)
- Exhaustion and fatigue assessment
- Fall/impact detection
- Real-time safety monitoring for climbing, mountaineering

### Military & Tactical
- Soldier stress and fatigue monitoring
- Blast detection and injury assessment
- Hypothermia/hyperthermia early warning
- Force protection and mission optimization

### Medical & Rehabilitation
- Post-operative patient monitoring
- Elderly fall detection
- Physical therapy exertion tracking
- Safer independent living

## Technical Specifications

### BLE Data Format
```
Service UUID: 0x180A (Device Information)

Characteristics:
├─ Heart Rate (0x2A37): 0-255 BPM
├─ SpO2 (0x2A5E): 0-100%
└─ Stress Level (0x181D): 0-100%

Update Rate: Every 1 second
Notification Type: Enabled on all characteristics
```

### Alert Thresholds
```
Heart Rate:
  Low: < 50 BPM
  High: > 150 BPM

Blood Oxygen:
  Critical: < 90%

Stress Level:
  Critical: > 80%

Impact Detection:
  Threshold: > 25 m/s²
```

### Power Budget (Estimated)
- ESP32: 80mA (active), 10mA (idle)
- MAX30102: 5mA
- MMA8451: 5mA
- GSR: <1mA
- OLED Display: 40mA
- LEDs: 20mA per LED
- **Total typical draw:** 150-200mA
- **Recommended battery:** 1000-2000mAh for 4-8 hour operation

## Getting Started

### Prerequisites
- Arduino IDE (latest version)
- ESP32 board support installed
- Required libraries:
  - Adafruit GFX
  - Adafruit SSD1306
  - Adafruit MMA8451
  - DFRobot BloodOxygen_S

### Installation

1. **Install Arduino IDE**
   - Download from https://www.arduino.cc/en/software

2. **Add ESP32 Board Support**
   - File → Preferences
   - Add to "Additional boards manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board Manager → Search "ESP32" → Install

3. **Install Required Libraries**
   - Sketch → Include Library → Manage Libraries
   - Search and install:
     - Adafruit GFX Library
     - Adafruit SSD1306
     - Adafruit MMA8451 Library
     - DFRobot BloodOxygen_S

4. **Configure Board Settings**
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Port → Select your COM port
   - Tools → Upload Speed → 115200

5. **Upload Code**
   - Copy sketch.ino into Arduino IDE
   - Click Upload (→ button)
   - Wait for "Done uploading"

6. **Verify Operation**
   - Tools → Serial Monitor
   - Set baud rate to 115200
   - You should see startup messages and sensor data

### Using with Light Blue App

1. Download **Light Blue** app (iOS/Android)
2. Open the app and scan for BLE devices
3. Connect to **"BioHelm_Helmet"**
4. Navigate to "Peripherals" tab
5. You should see three characteristics updating:
   - Heart Rate Measurement (0x2A37)
   - SpO2 (0x2A5E)
   - Stress Level (0x181D)
6. Click on each to view real-time values
7. Open "Log" tab to see continuous data stream

## Project Structure

```
biohelm/
├── README.md                 # This file
├── sketch.ino                # Main Arduino code with BLE
├── PROGRESS.md               # Development milestones
├── COMPONENT_LIST.md         # Detailed component specs
├── .gitignore                # Git ignore file
├── docs/
│   └── pinouts.md           # ESP32 pin assignments
├── hardware/
│   └── enclosure_design.md  # 3D-printed casing notes
└── media/
    └── [circuit diagrams, photos]
```

## Code Overview

### Main Sections
- **Sensor Reading:** Collects data from MAX30102, MMA8451, GSR
- **Data Processing:** Smoothing, validation, and alert detection
- **Display Update:** Refreshes OLED screen every 500ms
- **BLE Streaming:** Sends notifications every 1 second
- **Serial Logging:** Outputs formatted data for debugging

### Key Functions
- `readSensors()` - Reads all sensor inputs
- `updateDisplay()` - Updates OLED screen
- `logSensorData()` - Prints to Serial Monitor
- `updateBLEValues()` - Sends BLE notifications
- `isCriticalCondition()` - Checks alert thresholds
- `detectImpact()` - Identifies sudden acceleration

## Troubleshooting

### No Serial Output
- Check baud rate is set to 115200
- Verify correct COM port selected
- Ensure USB cable is data-capable (not charge-only)

### Sensor Initialization Fails
- Check I2C connections (SDA/SCL on GPIO 21/22)
- Verify I2C addresses match your sensors
- Look for error messages in Serial Monitor
- Try different USB port or cable

### BLE Not Connecting
- Ensure device name "BioHelm_Helmet" is visible
- Check Light Blue app has location/bluetooth permissions
- Restart ESP32 and app
- Try clearing app cache and reconnecting

### Inaccurate Readings
- Ensure MAX30102 has firm skin contact
- Check GSR electrode placement (fingertips optimal)
- Verify sensor is not moved during measurement
- Allow 10-15 seconds for sensor stabilization

## Future Enhancements

- WiFi data logging to cloud database
- Machine learning for fatigue prediction
- Temperature monitoring
- EEG integration for neural monitoring
- Mobile app for alerts and trends
- Data export for analysis
- Integration with emergency dispatch systems
- Predictive analytics for preemptive warnings

## Safety Disclaimers

- **Not a medical device:** BioHelm is a monitoring tool, not a diagnostic or therapeutic device
- **Always verify readings:** Cross-check with calibrated medical equipment
- **Emergency situations:** Do not rely solely on BioHelm for emergency response
- **Professional guidance:** Consult medical professionals for interpretation of data

## References & Resources

### Datasheets
- ESP32 Datasheet: https://docs.espressif.com/
- MAX30102 Datasheet: https://datasheetspdf.com/
- MMA8451 Datasheet: https://www.nxp.com/
- SSD1306 OLED Datasheet: https://cdn-shop.adafruit.com/

### Libraries
- Arduino Core for ESP32: https://github.com/espressif/arduino-esp32
- Adafruit SSD1306: https://github.com/adafruit/Adafruit_SSD1306
- DFRobot BloodOxygen: https://github.com/DFRobot/

### Learning Resources
- BLE GATT Specifications: https://www.bluetooth.com/
- PPG Sensor Technology: Various academic papers on photoplethysmography
- Arduino Documentation: https://www.arduino.cc/

## Team

**Developer:** Million Kibromokbay  
**Institution:** City St George's, University of London  
**Program:** Final Year BEng Biomedical & Healthcare Engineering

## License

This project is open source. Feel free to use, modify, and distribute for educational and non-commercial purposes.

---

**Last Updated:** 2024  
**Status:** Fully Functional Prototype
