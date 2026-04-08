# BioHelm: ESP32 Pinout & Connections Reference

## ESP32 DevKit Pin Layout

```
                        ┌─────────────────────┐
                        │    ESP32 DevKit     │
                        └─────────────────────┘

        GND  ← → 3V3        (Power Pins)
        D15  ← → D2
        D4   ← → D5
        D18  ← → D17
        D19  ← → D16
        D21  ← → D27
        D3   ← → D14
        D1   ← → D13
        D9   ← → D12
        D10  ← → D11
        D6   ← → D7
        D8   ← → D0
        GND  ← → 5V
```

---

## Current Wokwi Simulation Pinout

### Analog Inputs (12-bit ADC)
| Function | GPIO Pin | Range | Purpose |
|----------|----------|-------|---------|
| Heart Rate Input | GPIO 34 | 0-4095 | Simulates MAX30102 |
| Oxygen Input | GPIO 35 | 0-4095 | Simulates SpO₂ sensor |
| Stress Input | GPIO 32 | 0-4095 | Simulates GSR |

### Digital Inputs
| Function | GPIO Pin | State | Purpose |
|----------|----------|-------|---------|
| Crash Detection | GPIO 33 | HIGH/LOW | Simulates impact/crash |

### I2C Communication
| Device | SDA Pin | SCL Pin | Address | Purpose |
|--------|---------|---------|---------|---------|
| LCD Display | GPIO 21 | GPIO 22 | 0x27 | 16x2 I2C Display |

---

## Future Physical Hardware Pinout (To Be Updated)

### I2C Devices (Standard I2C Bus)
- **SDA:** GPIO 21 (Data line)
- **SCL:** GPIO 22 (Clock line)

#### I2C Connected Sensors
```
ESP32 (Master)
    ├── MAX30102 (Heart Rate + SpO₂)
    │   ├── SDA → GPIO 21
    │   ├── SCL → GPIO 22
    │   └── I2C Address: 0x57
    │
    ├── IMU (Motion Detection)
    │   ├── SDA → GPIO 21
    │   ├── SCL → GPIO 22
    │   └── I2C Address: 0x68 (MPU6050) or 0x77 (BMI160) - TBD
    │
    └── LCD Display (16x2)
        ├── SDA → GPIO 21
        ├── SCL → GPIO 22
        └── I2C Address: 0x27
```

### Analog Inputs (Physical Sensors)
| Sensor | GPIO Pin | ADC Channel | Voltage Range | Purpose |
|--------|----------|-------------|---------------|---------|
| GSR Electrodes | GPIO 34 | ADC1_CH6 | 0-3.3V | Stress measurement |
| TBD (Reserve) | GPIO 35 | ADC1_CH7 | 0-3.3V | Future analog sensor |

### Digital Outputs
| Function | GPIO Pin | Active Level | Purpose |
|----------|----------|--------------|---------|
| LED Alert - Green | GPIO 25 | HIGH | Normal operation |
| LED Alert - Yellow | GPIO 26 | HIGH | Warning condition |
| LED Alert - Red | GPIO 27 | HIGH | Critical alert |

### Power Pins
| Function | Pin | Voltage |
|----------|-----|---------|
| Positive Rail | 5V or 3V3 | 5V or 3.3V |
| Ground | GND | 0V (reference) |

---

## I2C Bus Configuration

### I2C Initialization Code
```cpp
// I2C Bus for sensors
#define I2C_SDA 21  // GPIO 21
#define I2C_SCL 22  // GPIO 22

// In setup():
Wire.begin(I2C_SDA, I2C_SCL);
Wire.setClock(100000);  // 100 kHz standard I2C speed
```

### Devices on I2C Bus
1. **MAX30102** (0x57) - Heart rate & SpO₂
2. **IMU** (0x68 or 0x77) - Motion detection
3. **LCD Display** (0x27) - Data display

### I2C Scanning
To find device addresses on bus:
```cpp
void scan_i2c() {
  Serial.println("Scanning I2C bus...");
  for (int address = 0; address < 128; address++) {
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at address: 0x");
      Serial.println(address, HEX);
    }
  }
}
```

---

## GPIO Pin Availability Summary

### In Use (Current)
- GPIO 21, 22: I2C (SDA/SCL)
- GPIO 34, 35, 32: Analog inputs (simulation)
- GPIO 33: Digital input (simulation)

### Available for Future Use
- GPIO 0, 2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 23, 25, 26, 27
- GPIO 9, 10, 11 (UART/SPI - use with caution)

### Reserved (Do Not Use)
- GPIO 6-8, 20, 24, 28-31 (Internal flash/reserved)
- GPIO 3 (Serial RX - if using USB)
- GPIO 1 (Serial TX - if using USB)

---

## Voltage Considerations

### Logic Levels
- **ESP32 I/O:** 3.3V logic
- **ADC Input Range:** 0-3.3V (max)
- **GPIO Output:** 3.3V when HIGH

### Power Supply Options
1. **Via USB:** 5V → Internal regulator → 3.3V to chip
2. **Via Battery:** Direct 3.3V or 3.7V (LiPo) with charger module
3. **Via 5V Rail + 3V3:** Only if board has voltage regulators

### Current Limits per Pin
- **GPIO Output:** ~12mA max per pin
- **Total GPIO current:** ~200mA max across all pins
- **ADC input impedance:** >1MΩ

---

## Typical Connection Wiring

### Power Distribution
```
Battery (3.7V LiPo)
    ↓
Charger Module (TP4056)
    ↓
ESP32 (VCC/GND)
    ├── 3V3 rail (for sensors)
    └── GND rail (common ground)
```

### Sensor Connection (I2C)
```
MAX30102 (SDA) → GPIO 21
MAX30102 (SCL) → GPIO 22
MAX30102 (VCC) → 3V3
MAX30102 (GND) → GND

[Same for IMU and LCD Display]
```

### GSR Electrodes (Analog)
```
Electrode 1 → GPIO 34 (signal)
Electrode 2 → GND (reference)
```

### LED Connections (with resistors)
```
ESP32 GPIO 25 → 220Ω Resistor → Green LED → GND
ESP32 GPIO 26 → 220Ω Resistor → Yellow LED → GND
ESP32 GPIO 27 → 220Ω Resistor → Red LED → GND
```

---

## Breadboard Layout Tips

1. **Power Rails:** Use outer pins for +3.3V and GND
2. **I2C Pull-ups:** May not be needed (MAX30102 & LCD often have built-in)
3. **Keep Short Wires:** Especially for I2C and analog signals
4. **Separate Signal & Power:** Run power and ground away from signal lines
5. **Label Everything:** Mark which wire goes where for easy troubleshooting

---

## Testing Pin Configuration

### Verify I2C Connection
```cpp
void check_i2c() {
  Wire.beginTransmission(0x27);  // LCD address
  int error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("LCD found on I2C bus!");
  } else {
    Serial.println("LCD not responding!");
  }
}
```

### Read ADC Values
```cpp
void check_adc() {
  int val = analogRead(GPIO_34);
  Serial.print("GPIO 34 value: ");
  Serial.println(val);  // Should be 0-4095
}
```

### Test GPIO Output
```cpp
void test_led(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);   // LED on
  delay(1000);
  digitalWrite(pin, LOW);    // LED off
}
```

---

## Future Expansion

### Serial Communication
- **UART 0:** GPIO 1 (TX), GPIO 3 (RX) - USB
- **UART 1:** GPIO 9 (TX), GPIO 10 (RX) - Available
- **UART 2:** GPIO 16 (TX), GPIO 17 (RX) - Available

### SPI Communication (if needed)
- **MISO:** GPIO 19
- **MOSI:** GPIO 23
- **SCK:** GPIO 18
- **CS:** GPIO 5 (configurable)

### Additional I2C Bus (if needed)
- Alternate SDA: GPIO 16 or 4
- Alternate SCL: GPIO 17 or 5

---

## Common Issues & Debugging

### I2C Not Working
- [ ] Check SDA/SCL connections
- [ ] Verify I2C address (0x27 for LCD)
- [ ] Check pull-up resistors (usually built-in)
- [ ] Ensure 3.3V power to sensors
- [ ] Run I2C scan to find actual addresses

### ADC Readings Unstable
- [ ] Add capacitor (0.1µF) near analog input
- [ ] Keep wires short and shielded
- [ ] Check for electromagnetic interference
- [ ] Verify power supply is stable

### GPIO Output Not Working
- [ ] Check pin mode is set to OUTPUT
- [ ] Verify GPIO isn't reserved/in-use
- [ ] Check current draw (12mA max per pin)
- [ ] Look for conflicting serial/SPI usage

---

## Reference Documents
- **ESP32 Datasheet:** https://docs.espressif.com/projects/esp-idf/en/release-v5.0/
- **Arduino IDE ESP32 Board:** https://github.com/espressif/arduino-esp32
- **I2C Standard:** https://en.wikipedia.org/wiki/I%C2%B2C

---

**Last Updated:** Week 2 - Simulation Phase  
**Status:** Ready for physical pin mapping upon component receipt
