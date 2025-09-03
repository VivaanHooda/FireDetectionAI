# 🔥 ESP32 based AI Fire Detection System

A comprehensive IoT fire detection system using ESP32 microcontrollers with AI-powered image recognition, multiple gas sensors, and real-time monitoring capabilities.

## 🌟 Features

- **AI-Powered Fire Detection**: Machine learning model for visual fire detection using ESP32-CAM
- **Multi-Sensor Approach**: Combines multiple detection methods for high accuracy
- **Real-time Monitoring**: Live data streaming via Blynk IoT platform
- **Gas Detection**: Multiple gas sensors (MQ2, MQ5, MQ7) for smoke, LPG, and CO detection
- **Temperature Monitoring**: Dual temperature sensing (ambient and object detection)
- **Flame Detection**: Dedicated flame sensor for immediate fire detection
- **Environmental Monitoring**: Humidity and air quality assessment
- **Wireless Communication**: UART communication between ESP32 modules
- **Mobile Alerts**: Push notifications through Blynk mobile app

## 🏗️ System Architecture

![Circuit Diagram](https://github.com/VivaanHooda/FireDetectionAI/blob/main/circuit-diagram.png)

The system consists of two ESP32 modules working in tandem:

### ESP32-CAM Module (Primary Detection)
- **AI Model**: Edge Impulse trained fire detection model
- **Camera**: OV2640 for real-time image capture and analysis
- **Gas Sensors**: MQ7 (CO) and MQ2 (smoke) sensors
- **Communication**: Sends detection data via UART to main module

### ESP32 Main Module (Data Processing & IoT)
- **Environmental Sensors**: BME680 (temperature, humidity, air quality)
- **Temperature Sensor**: MLX90614 infrared temperature sensor
- **Additional Gas Sensor**: MQ5 (LPG detection)
- **Flame Sensor**: Digital flame detection
- **IoT Connectivity**: Wi-Fi connection to Blynk platform
- **Alert System**: Buzzer and LED indicators

## 📋 Hardware Requirements

### ESP32-CAM Module
| Component | Model | Purpose |
|-----------|-------|---------|
| ESP32-CAM | AI-Thinker | Main controller with camera |
| MQ7 Sensor | Gas Sensor | Carbon monoxide detection |
| MQ2 Sensor | Gas Sensor | Smoke detection |
| LED | Indicator | Visual alert |

### ESP32 Main Module
| Component | Model | Purpose |
|-----------|-------|---------|
| ESP32 | Development Board | Main controller |
| BME680 | Environmental Sensor | Temperature, humidity, air quality |
| MLX90614 | IR Temperature Sensor | Non-contact temperature measurement |
| MQ5 Sensor | Gas Sensor | LPG detection |
| Flame Sensor | Digital Sensor | Direct flame detection |
| Buzzer | Audio Alert | Audible fire alarm |

### Common Components
- Breadboards/PCB
- Jumper wires
- Resistors (pull-up/pull-down)
- Power supply (5V/3.3V)

## 🔧 Pin Configuration

### ESP32-CAM Pin Mapping
```cpp
// Camera Pins (AI-Thinker ESP32-CAM)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
// ... (see code for complete mapping)

// Sensor Pins
#define MQ7_ANALOG_PIN    15
#define MQ2               2
#define LED               4
#define RX_PIN            13
#define TX_PIN            14
```

### ESP32 Main Pin Mapping
```cpp
// I2C Communication
#define I2C_SDA           21
#define I2C_SCL           22

// Sensor Pins
#define MQ5_PIN           5
#define FLAME_PIN         4
#define BUZZER_PIN        19
#define RX_PIN            16
#define TX_PIN            17
```

## 🚀 Installation & Setup

### 1. Arduino IDE Setup
```bash
# Install ESP32 board package in Arduino IDE
# Go to File > Preferences > Additional Board Manager URLs
# Add: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

### 2. Required Libraries
Install the following libraries through Arduino IDE Library Manager:

**For ESP32-CAM:**
- `ArduinoJson` by Benoit Blanchon
- Edge Impulse Arduino library (custom - see Edge Impulse documentation)

**For ESP32 Main:**
- `ArduinoJson` by Benoit Blanchon
- `Adafruit MLX90614 Library`
- `Adafruit BME680 Library`
- `Adafruit Unified Sensor`
- `Blynk` by Volodymyr Shymanskyy

### 3. Edge Impulse Model Setup
1. Create account at [Edge Impulse](https://edgeimpulse.com/)
2. Train your fire detection model
3. Download the Arduino library
4. Install in your Arduino libraries folder
5. Update the include path in the code: `#include <Your-Model-Name_inferencing.h>`

### 4. Blynk Configuration
1. Create a Blynk account and new project
2. Configure virtual pins (V0-V11) for data streams
3. Update the following in `esp32-main` code:
   ```cpp
   #define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
   #define BLYNK_TEMPLATE_NAME "YOUR_PROJECT_NAME"
   char auth[] = "YOUR_AUTH_TOKEN";
   char ssid[] = "YOUR_WIFI_SSID";
   char pass[] = "YOUR_WIFI_PASSWORD";
   ```

## 📊 Sensor Thresholds

The system uses the following default thresholds (adjustable in code):

| Sensor | Threshold | Unit | Purpose |
|--------|-----------|------|---------|
| MQ2 (Smoke) | 2000 | ADC Value | Smoke detection |
| MQ5 (LPG) | 600 | ADC Value | Gas leak detection |
| MQ7 (CO) | 1800 | ADC Value | Carbon monoxide detection |
| Temperature | 60 | °C | High temperature alert |
| Humidity | 30 | % | Low humidity (fire risk) |
| Gas Resistance | 15000 | Ω | Air quality indicator |

## 🧠 Fire Detection Logic

The system employs a multi-layered approach for fire detection:

1. **AI Visual Detection**: Camera-based ML model (70% confidence threshold)
2. **Multi-Sensor Scoring**: Points system based on sensor readings
3. **Flame Sensor**: Direct flame detection (immediate trigger)
4. **Threshold Logic**: Fire declared if ≥3 sensor conditions met OR flame detected OR AI confidence >70%

## 📱 Blynk Dashboard Configuration

### Virtual Pin Mapping
| Virtual Pin | Data | Widget Type |
|-------------|------|-------------|
| V0 | Safety Score | Gauge |
| V2 | MQ2 (Smoke) | Line Chart |
| V3 | MQ5 (LPG) | Line Chart |
| V4 | MQ7 (CO) | Line Chart |
| V5 | Flame Status | LED |
| V6 | Temperature | Gauge |
| V7 | Humidity | Gauge |
| V8 | Air Quality | Gauge |
| V9 | Object Temperature | Gauge |
| V10 | Ambient Temperature | Gauge |

## 🔄 Communication Protocol

The two ESP32 modules communicate via UART using JSON format:

**ESP32-CAM → ESP32-Main:**
```json
{
  "mq7": 1250,
  "mq2": 850,
  "safe": 0.85
}
```

## 🚨 Alert System

### Fire Detection Triggers
- Visual AI detection (>70% confidence)
- Flame sensor activation
- Multi-sensor threshold exceeded (≥3 conditions)
- Safety probability <20%

### Alert Actions
- LED indicator activation
- Buzzer alarm
- Blynk push notification
- Serial console logging

## 📈 Monitoring & Data Logging

- Real-time sensor data streaming
- Historical data charts in Blynk app
- Serial monitor debugging output
- JSON data format for easy integration

## 🛠️ Customization Options

### Adjustable Parameters
- Sensor threshold values
- AI confidence levels
- Heater cycle timing for MQ7
- Data transmission intervals
- Alert sensitivity levels

### Expandability
- Additional sensor integration
- Custom alert mechanisms
- Database logging
- Web dashboard integration
- SMS/Email notifications

## 🐛 Troubleshooting

### Common Issues

**Camera Initialization Failed**
```cpp
// Check camera pin connections
// Ensure adequate power supply (5V, 2A minimum)
// Verify ESP32-CAM model compatibility
```

**Sensor Reading Errors**
```cpp
// Check I2C connections for BME680/MLX90614
// Verify sensor power supply (3.3V/5V as required)
// Ensure proper pull-up resistors on I2C lines
```

**Wi-Fi Connection Issues**
```cpp
// Verify SSID and password
// Check network compatibility (2.4GHz required)
// Monitor signal strength
```

**Blynk Connection Problems**
```cpp
// Verify auth token
// Check internet connectivity
// Ensure correct template ID and name
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📞 Support

If you encounter any issues or have questions:

1. Check the [troubleshooting section](#-troubleshooting)
2. Review the [hardware setup guide](docs/hardware-setup.md)
3. Open an issue on GitHub
4. Provide detailed error messages and setup photos

## 🔬 Future Enhancements

- [ ] Integration with home automation systems
- [ ] Machine learning model updates over-the-air
- [ ] Battery-powered operation with sleep modes
- [ ] Multi-zone detection capability
- [ ] Integration with fire department alert systems
- [ ] Advanced data analytics and prediction
- [ ] Voice alerts and announcements

## 📚 References

- [ESP32-CAM Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
- [Edge Impulse Documentation](https://docs.edgeimpulse.com/)
- [Blynk Documentation](https://docs.blynk.io/)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)

## ⚠️ Safety Notice

This system is designed for educational and development purposes. For production fire safety applications:
- Ensure compliance with local fire safety regulations
- Consider redundant detection systems
- Regular calibration and maintenance required
- Professional installation recommended for critical applications

---

**Made with ❤️ for safer environments**
