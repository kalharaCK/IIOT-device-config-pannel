# ESP32 IoT Configuration Panel with GSM/LTE Integration

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-green.svg)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Version](https://img.shields.io/badge/Version-2.1.0-orange.svg)](https://github.com/yourusername/esp32-monitoring/releases)

A comprehensive ESP32-based IoT monitoring and configuration system with dual-mode WiFi (Access Point + Station) and GSM/LTE modem integration. Features a modern web-based dashboard for device management, real-time status monitoring, and cellular communication capabilities.

## 🚀 Features

### 🌐 **Dual-Mode WiFi Operation**
- **Access Point Mode**: ESP32 creates a WiFi hotspot for configuration
- **Station Mode**: Connects to existing WiFi networks
- **Captive Portal**: Automatic redirection for easy device setup
- **Real-time Network Scanning**: Discover and connect to available networks

### 📱 **GSM/LTE Integration**
- **SIMCom A76xx Support**: Compatible with A7600, A7600E, A7600F modems
- **Voice Calls**: Make and receive voice calls
- **SMS Messaging**: Send text messages with delivery confirmation
- **Network Information**: Real-time signal strength and carrier detection
- **APN Configuration**: Flexible data connection settings

### 🖥️ **Modern Web Dashboard**
- **Responsive Design**: Works on desktop, tablet, and mobile devices
- **Dark Theme**: Professional and easy on the eyes
- **Real-time Updates**: Live status monitoring with automatic refresh
- **Tabbed Interface**: Organized configuration sections
- **Caching System**: Optimized performance with 5-minute data caching

### ⚙️ **Configuration Management**
- **Persistent Storage**: SPIFFS-based configuration persistence
- **JSON-based Settings**: Human-readable configuration files
- **User Profiles**: Store user information and preferences
- **Backup & Restore**: Easy configuration backup and restoration

## 📋 Table of Contents

- [Hardware Requirements](#-hardware-requirements)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Usage](#-usage)
- [API Reference](#-api-reference)
- [Project Structure](#-project-structure)
- [Troubleshooting](#-troubleshooting)
- [Contributing](#-contributing)
- [License](#-license)

## 🔧 Hardware Requirements

### **ESP32 Development Board**
- ESP32 DevKit (any variant)
- Minimum 4MB Flash memory
- 512KB RAM recommended

### **GSM/LTE Modem**
- SIMCom A76xx series (A7600, A7600E, A7600F)
- Compatible with 2G/3G/4G networks
- SIM card slot (micro-SIM or nano-SIM)

### **Wiring Connections**
```
ESP32          GSM Modem
------         ---------
GPIO 16 (RX2)  → TX
GPIO 17 (TX2)  → RX
GND            → GND
3.3V/5V        → VCC (check modem requirements)
```

### **Power Requirements**
- ESP32: 3.3V, ~240mA (active)
- GSM Modem: 3.3V-5V, ~2A (peak during transmission)
- **Total**: ~2.5A peak current requirement

## 📦 Installation

### **Prerequisites**
- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software)
- [Git](https://git-scm.com/) for version control

### **1. Clone the Repository**
```bash
git clone https://github.com/yourusername/esp32-monitoring.git
cd esp32-monitoring
```

### **2. Install Dependencies**
The project uses PlatformIO for dependency management. Dependencies are automatically installed when you build the project:

```ini
lib_deps = 
    me-no-dev/AsyncTCP @ ^1.1.1
    bblanchon/ArduinoJson@^7.4.2
```

### **3. Build and Upload**
```bash
# Using PlatformIO
pio run --target upload

# Or using Arduino IDE
# 1. Open src/main.cpp in Arduino IDE
# 2. Install required libraries
# 3. Select ESP32 board and upload
```

### **4. Monitor Serial Output**
```bash
# PlatformIO
pio device monitor

# Arduino IDE
# Use Serial Monitor at 115200 baud
```

## ⚙️ Configuration

### **Initial Setup**
1. **Power on** the ESP32 with the GSM modem connected
2. **Connect** to the WiFi network "ESP32-AccessPoint" (password: 12345678)
3. **Open** your browser and navigate to `http://192.168.4.1`
4. **Configure** your settings through the web dashboard

### **WiFi Configuration**
- **Access Point**: Customize SSID and password
- **Station Mode**: Connect to your home/office WiFi
- **Network Scanning**: Discover available networks
- **Connection Testing**: Verify connectivity

### **GSM Configuration**
- **APN Settings**: Configure for your carrier
- **SIM Card**: Ensure proper insertion and activation
- **Network Testing**: Verify signal strength and registration

### **User Profile**
- **Personal Information**: Name, email, phone number
- **Notification Settings**: Configure alert preferences
- **Test Contacts**: Set up test phone numbers

## 🎯 Usage

### **Web Dashboard Access**
- **Local Access**: `http://192.168.4.1` (when connected to ESP32 AP)
- **Network Access**: `http://[ESP32_IP]` (when connected to your WiFi)
- **Mobile Friendly**: Responsive design works on all devices

### **Dashboard Tabs**

#### **1. User Management**
- Configure user profile information
- View system information and status
- Manage user preferences

#### **2. WiFi Configuration**
- Scan for available networks
- Connect to WiFi networks
- Monitor connection status
- View connected devices

#### **3. GSM Settings**
- Configure APN settings
- Test SMS messaging
- Make test voice calls
- Monitor signal strength
- View network information

### **Real-time Monitoring**
- **Status Updates**: Automatic refresh every 30 seconds
- **Signal Strength**: Live GSM signal monitoring
- **Network Info**: Current carrier and connection status
- **Connected Devices**: Number of devices on AP

## 📚 API Reference

### **System Status**
```http
GET /api/status
```
Returns comprehensive system status including WiFi and GSM information.

### **WiFi Management**
```http
GET /api/wifi/scan          # Scan for networks
POST /api/wifi/connect      # Connect to network
POST /api/wifi/disconnect   # Disconnect from network
```

### **GSM Operations**
```http
GET /api/gsm/signal         # Get signal strength
GET /api/gsm/network        # Get network information
POST /api/gsm/sms           # Send SMS
POST /api/gsm/call          # Make voice call
POST /api/gsm/setapn        # Configure APN
```

### **Configuration Management**
```http
GET /api/load/user          # Load user profile
POST /api/save/user         # Save user profile
GET /api/load/gsm           # Load GSM settings
POST /api/save/gsm          # Save GSM settings
```

### **System Control**
```http
POST /api/reboot            # Restart device
```

## 📁 Project Structure

```
esp32-monitoring/
├── src/
│   ├── main.cpp              # Main application code
│   ├── GSM_Test.h            # GSM library header
│   ├── GSM_Test.cpp          # GSM library implementation
│   └── dashboard_html.h      # Embedded web dashboard
├── include/                  # Additional headers
├── lib/                      # Custom libraries
├── test/                     # Unit tests
├── platformio.ini           # PlatformIO configuration
└── README.md                # This file
```

### **Key Components**

#### **main.cpp**
- Core application logic
- Web server and API endpoints
- WiFi and GSM management
- Configuration persistence

#### **GSM_Test Library**
- AT command interface
- SMS and voice call handling
- Network information retrieval
- Signal strength monitoring

#### **dashboard_html.h**
- Complete web interface
- Responsive CSS styling
- JavaScript functionality
- Real-time data updates

## 🔍 Troubleshooting

### **Common Issues**

#### **ESP32 Won't Start**
- Check power supply (minimum 2.5A)
- Verify wiring connections
- Check serial output for error messages

#### **WiFi Connection Issues**
- Ensure SSID and password are correct
- Check signal strength
- Verify network security settings

#### **GSM Modem Problems**
- Verify SIM card insertion
- Check SIM card activation
- Ensure proper power supply
- Verify AT command communication

#### **Web Dashboard Not Loading**
- Check IP address
- Clear browser cache
- Try different browser
- Check firewall settings

### **Debug Information**
Enable serial monitoring at 115200 baud to view debug information:
- System initialization
- WiFi connection status
- GSM modem responses
- API request logs

### **Reset to Factory Defaults**
1. Hold BOOT button while powering on
2. Access recovery mode
3. Clear SPIFFS partition
4. Re-upload firmware


### **Development Setup**
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### **Code Style**
- Follow Arduino/ESP32 coding conventions
- Add comments for complex logic
- Use meaningful variable names
- Test on actual hardware

### **Reporting Issues**
- Use GitHub Issues
- Include hardware details
- Provide serial output
- Describe steps to reproduce


**Made with ❤️ for the IoT community**
