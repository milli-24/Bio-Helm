/*
  ==========================================
   BIOHELM HEALTH MONITOR WITH BLE STREAMING
   Sensors:
   - OLED Display (SSD1306)
   - Pulse Oximeter / Heart Rate (MAX30102)
   - Accelerometer (MMA8451)
   - Analog Sensor (GSR - Stress)
   
   BLE Streaming to Light Blue App
  ==========================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MMA8451.h>
#include "DFRobot_BloodOxygen_S.h"
#include <math.h>
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"

/* ============= CONFIG ============= */

#define ANALOG_SENSOR_PIN 34
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
#define SENSOR_ADDRESS 0x57
#define ACCEL_ADDRESS 0x1D

#define CRITICAL_HR_LOW 50
#define CRITICAL_HR_HIGH 150
#define CRITICAL_SPO2_LOW 90
#define CRITICAL_STRESS_HIGH 80

#define SERIAL_BAUD 115200
#define ENABLE_DETAILED_LOGGING false

/* ============= BLE CONFIG ============= */

#define SERVICE_UUID        "180A"
#define HEARTRATE_CHAR_UUID "2A37"
#define SPO2_CHAR_UUID      "2A5E"
#define STRESS_CHAR_UUID    "181D"

const char *bleName = "BioHelm_Helmet";

/* ============= OBJECTS ============= */

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();
DFRobot_BloodOxygen_S_I2C pulseSensor(&Wire, SENSOR_ADDRESS);

BLEServer* pServer = NULL;
BLECharacteristic* pHeartRateChar = NULL;
BLECharacteristic* pSpO2Char = NULL;
BLECharacteristic* pStressChar = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

/* ============= VARIABLES ============= */

unsigned long lastDisplayUpdate = 0;
unsigned long lastSensorRead = 0;
unsigned long lastSerialLog = 0;
unsigned long lastBLEUpdate = 0;
unsigned long startTime = 0;

const int displayInterval = 500;
const int sensorInterval = 60;
const int serialLogInterval = 1000;
const int bleUpdateInterval = 1000;

float smoothAnalogValue = 2000;
float smoothHeartRate = 0;
float smoothSpO2 = 0;
float smoothMotion = 0;

int sensorErrorCount = 0;

/* ============= BLE SERVER CALLBACKS ============= */

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("✓ BLE Connected!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("✗ BLE Disconnected!");
    }
};

/* ============= SETUP ============= */

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Wire.begin();
  startTime = millis();

  printSerialHeader();
  
  setupDisplay();
  setupPulseSensor();
  setupAccelerometer();

  pinMode(ANALOG_SENSOR_PIN, INPUT);
  
  checkSensorHealth();
  
  // Setup BLE
  setupBLE();
  
  Serial.println("\n✓ System Initialization Complete");
  Serial.println("Waiting for sensor data...\n");
}

/* ============= MAIN LOOP ============= */

void loop() {
  readSensors();

  if (millis() - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = millis();
    updateDisplay();
  }

  if (millis() - lastSerialLog >= serialLogInterval) {
    lastSerialLog = millis();
    logSensorData();
  }

  if (millis() - lastBLEUpdate >= bleUpdateInterval) {
    lastBLEUpdate = millis();
    updateBLEValues();
  }

  // Handle BLE disconnection
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("BLE Advertising restarted...");
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}

/* ============= SENSOR READING FUNCTIONS ============= */

void readSensors() {
  int rawValue = analogRead(ANALOG_SENSOR_PIN);
  smoothAnalogValue = (smoothAnalogValue * 0.9) + (rawValue * 0.1);

  if (millis() - lastSensorRead >= sensorInterval) {
    lastSensorRead = millis();
    
    pulseSensor.getHeartbeatSPO2();
    int rawHR = pulseSensor._sHeartbeatSPO2.Heartbeat;
    int rawSpO2 = pulseSensor._sHeartbeatSPO2.SPO2;
    
    if (rawHR > 30 && rawHR < 220) {
      smoothHeartRate = (smoothHeartRate * 0.7) + (rawHR * 0.3);
    }
    if (rawSpO2 > 50 && rawSpO2 <= 100) {
      smoothSpO2 = (smoothSpO2 * 0.7) + (rawSpO2 * 0.3);
    }
  }

  smoothMotion = getMotionMagnitude();
}

int getHeartRate() {
  int bpm = (int)smoothHeartRate;
  if (bpm >= 40 && bpm <= 200) {
    return bpm;
  }
  return 0;
}

int getSpO2() {
  int spo2 = (int)smoothSpO2;
  if (spo2 >= 80 && spo2 <= 100) {
    return spo2;
  }
  return 0;
}

float getMotionMagnitude() {
  sensors_event_t event;
  
  if (!accelerometer.getEvent(&event)) {
    return 0;
  }

  return sqrt(
    event.acceleration.x * event.acceleration.x +
    event.acceleration.y * event.acceleration.y +
    event.acceleration.z * event.acceleration.z
  );
}

/* ============= DISPLAY FUNCTIONS ============= */

void updateDisplay() {
  display.clearDisplay();

  if (isCriticalCondition()) {
    displayCriticalAlert();
  } else {
    displayHeader();
    displayVitals();
    displayStatusBar();
  }

  display.display();
}

void displayHeader() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("BioHelm");

  // Show BLE status
  display.setCursor(100, 0);
  if (deviceConnected) {
    display.print("BLE");
  } else {
    display.print("---");
  }

  display.drawFastHLine(0, 10, 128, WHITE);
}

void displayVitals() {
  int bpm = getHeartRate();
  int spo2 = getSpO2();
  int level = calculateStressLevel();
  float motion = smoothMotion;

  display.setTextSize(1);
  
  display.setCursor(0, 15);
  display.print("BPM: ");
  display.print((bpm > 0) ? bpm : 0);

  display.setCursor(70, 15);
  display.print("O2: ");
  display.print((spo2 > 0) ? spo2 : 0);
  display.print("%");

  display.setCursor(0, 30);
  display.print("Stress: ");
  display.print(level);
  display.print("%");

  display.setCursor(0, 45);
  display.print("Motion: ");
  display.print((int)motion);
}

void displayStatusBar() {
  int level = calculateStressLevel();

  display.drawRect(0, 58, 128, 6, WHITE);

  int width = map(level, 0, 100, 0, 126);
  display.fillRect(1, 59, width, 4, WHITE);
}

void displayCriticalAlert() {
  display.setTextSize(2);
  display.setCursor(15, 15);
  display.print("ALERT!");
  
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.print("Critical condition");
  display.setCursor(15, 50);
  display.print("detected");
}

/* ============= SERIAL MONITOR LOGGING ============= */

void printSerialHeader() {
  Serial.println("\n");
  Serial.println("=====================================");
  Serial.println("     BioHelm Health Monitor");
  Serial.println("     Real-time Sensor Data + BLE");
  Serial.println("=====================================");
  Serial.println("Time (ms) | HR (bpm) | SpO2 (%) | Stress (%) | Motion | Status");
  Serial.println("---------------------------------------------------------------------");
}

void logSensorData() {
  int bpm = getHeartRate();
  int spo2 = getSpO2();
  int stress = calculateStressLevel();
  float motion = smoothMotion;
  unsigned long uptime = millis() - startTime;

  Serial.print(uptime);
  Serial.print(" | ");
  Serial.print((bpm > 0) ? bpm : 0);
  Serial.print(" | ");
  Serial.print((spo2 > 0) ? spo2 : 0);
  Serial.print(" | ");
  Serial.print(stress);
  Serial.print(" | ");
  Serial.print((int)motion);
  Serial.print(" | ");

  if (isCriticalCondition()) {
    Serial.print("⚠ CRITICAL");
  } else if (detectImpact(motion)) {
    Serial.print("⚠ IMPACT");
  } else {
    Serial.print("✓ Normal");
  }

  if (deviceConnected) {
    Serial.print(" [BLE: Connected]");
  }

  Serial.println();

  if (ENABLE_DETAILED_LOGGING) {
    logDetailedData(bpm, spo2, stress, motion);
  }
}

void logDetailedData(int bpm, int spo2, int stress, float motion) {
  Serial.print("  Raw: HR=");
  Serial.print(pulseSensor._sHeartbeatSPO2.Heartbeat);
  Serial.print(" SpO2=");
  Serial.print(pulseSensor._sHeartbeatSPO2.SPO2);
  Serial.print(" GSR=");
  Serial.print((int)smoothAnalogValue);
  Serial.print(" Motion=");
  Serial.println(motion);
}

/* ============= BLE FUNCTIONS ============= */

void setupBLE() {
  Serial.print("Setting up BLE... ");
  
  BLEDevice::init(bleName);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Heart Rate Characteristic
  pHeartRateChar = pService->createCharacteristic(
                      HEARTRATE_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pHeartRateChar->addDescriptor(new BLE2902());

  // SpO2 Characteristic
  pSpO2Char = pService->createCharacteristic(
                      SPO2_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pSpO2Char->addDescriptor(new BLE2902());

  // Stress Level Characteristic
  pStressChar = pService->createCharacteristic(
                      STRESS_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pStressChar->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  
  Serial.println("✓ OK");
  Serial.print("BLE Device Name: ");
  Serial.println(bleName);
}

void updateBLEValues() {
  if (deviceConnected) {
    int bpm = getHeartRate();
    int spo2 = getSpO2();
    int stress = calculateStressLevel();

    // Send Heart Rate
    uint8_t hrData[2] = {0x00, (uint8_t)bpm};
    pHeartRateChar->setValue(hrData, 2);
    pHeartRateChar->notify();

    // Send SpO2
    uint8_t spo2Data[2] = {0x00, (uint8_t)spo2};
    pSpO2Char->setValue(spo2Data, 2);
    pSpO2Char->notify();

    // Send Stress
    uint8_t stressData[2] = {0x00, (uint8_t)stress};
    pStressChar->setValue(stressData, 2);
    pStressChar->notify();
  }
}

/* ============= ALERT & LOGIC FUNCTIONS ============= */

bool isCriticalCondition() {
  int bpm = getHeartRate();
  int spo2 = getSpO2();
  int stress = calculateStressLevel();

  if (bpm > 0 && (bpm < CRITICAL_HR_LOW || bpm > CRITICAL_HR_HIGH)) {
    return true;
  }
  if (spo2 > 0 && spo2 < CRITICAL_SPO2_LOW) {
    return true;
  }
  if (stress > CRITICAL_STRESS_HIGH) {
    return true;
  }

  return false;
}

bool detectImpact(float motion) {
  return motion > 25.0;
}

int calculateStressLevel() {
  int level = map((int)smoothAnalogValue, 3500, 800, 0, 100);
  return constrain(level, 0, 100);
}

/* ============= SETUP HELPER FUNCTIONS ============= */

void setupDisplay() {
  Serial.print("Initializing OLED Display... ");
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("❌ FAILED");
    sensorErrorCount++;
    return;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  
  Serial.println("✓ OK");
}

void setupPulseSensor() {
  Serial.print("Initializing Pulse/SpO2 Sensor... ");
  
  if (!pulseSensor.begin()) {
    Serial.println("❌ FAILED");
    sensorErrorCount++;
    return;
  }

  pulseSensor.sensorStartCollect();
  Serial.println("✓ OK");
}

void setupAccelerometer() {
  Serial.print("Initializing Accelerometer... ");
  
  if (!accelerometer.begin(ACCEL_ADDRESS)) {
    Serial.println("❌ FAILED");
    sensorErrorCount++;
    return;
  }

  accelerometer.setRange(MMA8451_RANGE_2_G);
  Serial.println("✓ OK");
}

void checkSensorHealth() {
  Serial.println("\n--- Sensor Health Check ---");
  
  Serial.print("OLED Display: ");
  if (display.width() > 0) {
    Serial.println("✓ OK");
  } else {
    Serial.println("❌ NO SIGNAL");
  }

  Serial.print("Pulse/SpO2 Sensor: ");
  pulseSensor.getHeartbeatSPO2();
  Serial.print("✓ OK (HR=");
  Serial.print(pulseSensor._sHeartbeatSPO2.Heartbeat);
  Serial.print(" SpO2=");
  Serial.print(pulseSensor._sHeartbeatSPO2.SPO2);
  Serial.println("%)");

  Serial.print("Accelerometer: ");
  sensors_event_t event;
  if (accelerometer.getEvent(&event)) {
    Serial.print("✓ OK (X=");
    Serial.print(event.acceleration.x);
    Serial.println(")");
  } else {
    Serial.println("❌ NO SIGNAL");
  }

  Serial.print("Analog Sensor (GSR): ");
  int analogValue = analogRead(ANALOG_SENSOR_PIN);
  Serial.print("✓ OK (Value=");
  Serial.print(analogValue);
  Serial.println(")");

  if (sensorErrorCount > 0) {
    Serial.println("\n⚠ WARNING: Some sensors failed to initialize!");
    Serial.println("Check connections and I2C addresses\n");
  }
}