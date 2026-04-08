# BioHelm Project Progress

## Week 1-2: Circuit Simulation

### Status: ✅ COMPLETE

#### Testing Completed
- [x] Wokwi circuit created and functional
- [x] ESP32 microcontroller responding correctly
- [x] All three potentiometers reading analog inputs (0-4095 range)
- [x] Analog values mapping correctly to realistic health metrics
  - Heart Rate: 60-180 BPM range
  - Blood Oxygen: 80-100% range
  - Stress Level: 0-100% range
- [x] LCD display (16x2 I2C) initializing and displaying data
- [x] Display cycling through all three vital sign screens smoothly
- [x] Crash detection switch triggering correctly
- [x] Crash warning display flashing as intended
- [x] System responsive to crash input during all operational phases
- [x] Serial communication initialized (115200 baud)

#### What's Working
- **Sensor Input:** Potentiometers simulate MAX30102, IMU, and GSR sensors accurately
- **Data Processing:** Raw ADC readings mapped to physiologically realistic ranges
- **Display Output:** LCD clearly shows all vital signs with proper formatting
- **Safety Priority:** Crash detection interrupts normal operation immediately
- **Code Quality:** No compilation errors, clean logic flow, proper commenting

#### Simulation Link
**[BioHelm Wokwi Simulation](https://wokwi.com/projects/456304586708837377)**

### Key Code Features
- **Responsive Crash Detection:** Checks crash sensor every 100ms even during delays
- **Multi-screen Display:** Cycles through Heart Rate → Oxygen → Stress with 2-second intervals
- **Value Mapping:** Realistic health metric ranges from raw ESP32 ADC readings
- **Clean Initialization:** System startup message with 2-second confirmation delay

---

## Week 3+: Physical Hardware Assembly (Upcoming)

### Next Milestones

#### Phase 2: Component Procurement
- [ ] Contact project leads: Abdulbaset.Aziemie@citystgeorges.ac.uk
- [ ] Receive physical components kit
- [ ] Verify all components match component list
- [ ] Organize workspace and tools

#### Phase 3: Breadboard Prototyping
- [ ] Set up breadboard with ESP32
- [ ] Integrate MAX30102 (heart rate/SpO₂ sensor)
  - [ ] Verify I2C communication
  - [ ] Test sensor readings
  - [ ] Calibrate output ranges
- [ ] Integrate IMU (motion & impact detection)
  - [ ] Verify I2C/SPI connection (TBD which protocol)
  - [ ] Test acceleration/impact detection
  - [ ] Calibrate thresholds
- [ ] Integrate GSR electrodes (stress/skin conductance)
  - [ ] Connect to analog input
  - [ ] Verify conductance readings
  - [ ] Test baseline variation
- [ ] Test battery and charger module
  - [ ] Verify safe charging
  - [ ] Test power management
- [ ] Connect LEDs for visual alerts
- [ ] Full system integration testing

#### Phase 4: Helmet Integration
- [ ] Prepare helmet structure
- [ ] Mount components securely
- [ ] Route wiring safely
- [ ] Test fit and comfort
- [ ] Ensure all sensors make proper contact

#### Phase 5: Wireless Transmission
- [ ] Implement WiFi/BLE transmission on ESP32
- [ ] Create data dashboard for remote monitoring
- [ ] Test transmission reliability
- [ ] Optimize power consumption

#### Phase 6: Final Testing & Validation
- [ ] Safety testing in simulated high-risk scenarios
- [ ] Durability testing
- [ ] User comfort validation
- [ ] Final documentation and user manual

---

## Notes & Observations

### Current System Behavior
- System initializes cleanly with "System Starting..." message
- Display transitions are smooth with proper timing
- Crash detection takes priority over all other operations
- Potentiometer adjustments in Wokwi reflect in real-time on LCD
- No lag or timing issues observed

### Known Simulation Limitations
- Potentiometers simulate real sensor data (will change with actual sensors)
- Slide switch simulates crash (will use real IMU accelerometer)
- No actual wireless transmission in simulation (to be added in Phase 5)
- Battery simulation not included (power management to be implemented later)

### Questions for Next Phase
- [ ] Which specific IMU model? (MPU6050, BMI160, etc.)
- [ ] What's the LCD I2C address? (Currently 0x27 - verify with hardware)
- [ ] Wireless protocol preference? (WiFi vs BLE vs both)
- [ ] Target sampling rate for real sensors?
- [ ] Alert thresholds for critical vital signs?

---

## Testing Methodology

### Simulation Testing Approach
1. **Initialization Test:** Verify system boots and displays startup message
2. **Sensor Input Test:** Adjust potentiometers and confirm value mapping
3. **Display Output Test:** Verify LCD shows all screens in correct order
4. **Safety Priority Test:** Trigger crash switch and confirm interrupt behavior
5. **Response Time Test:** Measure time to crash detection (should be <100ms)

### Physical Hardware Testing (To Be Documented)
- Component-by-component integration testing
- Sensor calibration and baseline establishment
- System latency and sampling rate validation
- Power consumption profiling
- Safety threshold verification
- Environmental stress testing

---

## Team Information

### Current Team
- Primary Developer: [Your Name]
- Status: Solo (Colleague - TBD availability)

### Key Contacts
- **Project Lead:** Abdulbaset.Aziemie@citystgeorges.ac.uk
- **Academic Supervisor:** Fouzia.Anser@city.ac.uk

### Important Notes from Brief
- Max team size: 3 members
- All components provided free of charge
- Paired with GoKart Team (helmet size must match driver)
- Serious candidates only - material wastage not appreciated
- Once physical components received: assembly and soldering required

---

## Resources & References

### Tools Used
- **Simulator:** Wokwi (https://wokwi.com/)
- **IDE:** Arduino IDE
- **Microcontroller:** ESP32 DevKit
- **Display:** 16x2 I2C LCD (Address: 0x27)

### Datasheets to Review (Physical Phase)
- ESP32 Datasheet (pinout, I2C/SPI, ADC specs)
- MAX30102 (Heart Rate/SpO₂ sensor)
- IMU Sensor (specific model TBD)
- LCD 16x2 I2C (display timing, commands)

### Useful Documentation
- Arduino IDE: https://www.arduino.cc/
- ESP32 Resources: https://docs.espressif.com/
- Wokwi Simulator: https://docs.wokwi.com/
- I2C Communication: General protocol for sensor communication

---

**Last Updated:** Week 2 - Simulation Complete  
**Next Review:** Upon component receipt
