# ESP32 Configuration Panel with Double Reset Detection

[![Version](https://img.shields.io/badge/version-2.3.0-blue.svg)](https://github.com/yourusername/esp32-config-panel)
[![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino](https://img.shields.io/badge/Arduino-compatible-brightgreen.svg)](https://www.arduino.cc/)

A comprehensive dual-mode configuration panel for ESP32 with WiFi and GSM management capabilities. Features intelligent dashboard switching via double reset detection mechanism.

## 📋 Table of Contents

- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Dependencies](#software-dependencies)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [API Documentation](#api-documentation)
- [Dashboard Modes](#dashboard-modes)
- [File Structure](#file-structure)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## ✨ Features

### Core Functionality
- **Dual Dashboard System**: Switch between Main and Email configuration dashboards
- **Double Reset Detection**: Smart mode switching by pressing reset twice within 3 seconds
- **WiFi Management**: 
  - Access Point (AP) mode for configuration portal
  - Station (STA) mode for internet connectivity
  - Network scanning and connection management
- **GSM/Cellular Support**:
  - SIM800L/SIM900A modem integration
  - Voice calls with 10-second auto-hangup
  - SMS messaging capabilities
  - Network signal monitoring and carrier detection
- **Email Functionality**:
  - SMTP email sending via GSM network
  - Configurable Gmail/SMTP server support
  - Secure app password authentication
- **Sensor Monitoring**: Real-time environmental data (temperature, humidity, light)
- **Captive Portal**: Automatic redirection for easy device setup
- **Persistent Storage**: Configuration saved to SPIFFS filesystem
- **RESTful API**: Complete HTTP API for all operations

### Dashboard Modes

#### 🏠 Main Dashboard (Mode 1)
- WiFi network management (scan, connect, disconnect)
- GSM operations (calls, SMS, signal monitoring)
- Sensor data visualization
- User profile management
- System information display

#### 📧 Email Configuration Dashboard (Mode 2)
- SMTP server configuration
- Email account setup
- Access Point (AP) credentials management
- Email testing via GSM
- Sensor monitoring

## 🔧 Hardware Requirements

### Required Components
- **ESP32 Development Board** (DevKit, WROOM, or compatible)
- **GSM Modem Module** (SIM800L, SIM900A, or compatible)
- **SIM Card** with active cellular plan
- **Power Supply** (3.7V for GSM module, 5V for ESP32)

### Wiring Diagram

```
ESP32                    GSM Module (SIM800L)
─────                    ──────────────────
GPIO 17 (TX)  ────────►  RX
GPIO 16 (RX)  ◄────────  TX
GND          ────────►  GND
3.7-4.2V     ────────►  VCC
```

**Important Notes:**
- GSM modules require stable 2A current supply
- Use separate power supply for GSM module (not from ESP32)
- Add 100-1000µF capacitor near GSM module VCC pin
- Ensure proper antenna connection

## 📦 Software Dependencies

### Arduino Libraries

```cpp
// Core Libraries (built-in with ESP32 board package)
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>

// Required External Libraries
#include <ArduinoJson.h>        // v6.x recommended

// Project Files (included in repository)
#include "GSM_Test.h"           // GSM modem interface
#include "SMTP.h"               // SMTP email client
#include "DRD_Manager.h"        // Double reset detector
#include "dashboard_html.h"     // Main dashboard UI
#include "config_html.h"        // Email config UI
```

### Installing Dependencies

**Via Arduino Library Manager:**
1. Open Arduino IDE
2. Go to `Sketch → Include Library → Manage Libraries`
3. Search and install:
   - ArduinoJson (by Benoit Blanchon)

**ESP32 Board Support:**
1. Add to `File → Preferences → Additional Board Manager URLs`:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. Install via `Tools → Board → Boards Manager`: "ESP32 by Espressif Systems"

## 🚀 Installation

### 1. Clone Repository

```bash
git clone https://github.com/yourusername/esp32-config-panel.git
cd esp32-config-panel
```

### 2. Project Structure

```
esp32-config-panel/
├── esp32-config-panel.ino    # Main sketch file
├── GSM_Test.h                 # GSM modem library
├── GSM_Test.cpp
├── SMTP.h                     # SMTP email library
├── SMTP.cpp
├── DRD_Manager.h              # Double reset detection
├── DRD_Manager.cpp
├── dashboard_html.h           # Main dashboard (compressed)
├── config_html.h              # Email config dashboard (compressed)
├── README.md                  # This file
└── LICENSE
```

### 3. Configuration

Open `esp32-config-panel.ino` and verify these settings:

```cpp
// Default Access Point Credentials
#define DEFAULT_AP_SSID "Config panel"
#define DEFAULT_AP_PASS "12345678"

// Device Information
#define DEVICE_MODEL "ESP32 DevKit"
#define FIRMWARE_VERSION "v2.3.0"

// Double Reset Detection
#define DRD_TIMEOUT 3000  // 3 seconds

// GSM Serial Configuration
GSM_Test gsmModem(Serial2, 16, 17, 115200);  // RX=16, TX=17
```

### 4. Upload to ESP32

1. Connect ESP32 to computer via USB
2. Select board: `Tools → Board → ESP32 Dev Module`
3. Select port: `Tools → Port → [Your ESP32 Port]`
4. Click Upload button or `Ctrl+U`

### 5. Monitor Serial Output

Open Serial Monitor (`Ctrl+Shift+M`) at **115200 baud** to view startup logs:

```
╔════════════════════════════════════════╗
║  ESP32 Configuration Panel with DRD   ║
╚════════════════════════════════════════╝
✓ Single reset detected
✓ Loading MAIN Dashboard (dashboard_html.h)
✓ SPIFFS mounted successfully
✓ Access Point Started:
  SSID: Config panel
  IP: 192.168.4.1
  Mode: MAIN Dashboard
╚════════════════════════════════════════╝
```

## 📖 Usage

### Initial Setup

1. **Power On**: Connect ESP32 to power source
2. **Connect to WiFi**: 
   - Look for WiFi network: `Config panel`
   - Password: `12345678`
3. **Access Dashboard**: 
   - Automatically redirected to `http://192.168.4.1`
   - Or manually navigate to IP address

### Switching Between Dashboards

**Method: Double Reset Detection (DRD)**

1. Press the **RESET** button on ESP32
2. Wait for device to start (LED blinks)
3. Press **RESET** button again within **3 seconds**
4. Dashboard mode switches:
   - Main → Email Configuration
   - Email Configuration → Main

### Main Dashboard Operations

#### WiFi Network Management

```javascript
// Scan for networks
GET /api/wifi/scan

// Get scan results
GET /api/wifi/scan/results

// Connect to network
POST /api/wifi/connect
{
  "ssid": "YourNetwork",
  "password": "yourpassword"
}

// Disconnect
POST /api/wifi/disconnect
```

#### GSM Operations

```javascript
// Check signal strength
GET /api/gsm/signal?force=true

// Get network info
GET /api/gsm/network

// Make voice call
POST /api/gsm/call
{
  "phoneNumber": "+94719792341"
}

// Send SMS
POST /api/gsm/sms
{
  "phoneNumber": "+94719792341",
  "message": "Hello from ESP32!"
}
```

### Email Configuration Dashboard

#### Configure SMTP Settings

```javascript
POST /api/save/email
{
  "smtpHost": "smtp.gmail.com",
  "smtpPort": 465,
  "emailAccount": "your-email@gmail.com",
  "emailPassword": "your-app-password",
  "senderName": "ESP32 Device"
}
```

#### Send Email via GSM

```javascript
POST /api/email/gsm/send
{
  "to": "recipient@example.com",
  "subject": "Temperature Alert",
  "content": "Temperature exceeded threshold: 32.5°C"
}
```

## 🔌 API Documentation

### Common Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/status` | GET | System status (WiFi, GSM, mode) |
| `/api/sensors` | GET | Sensor readings |
| `/api/system/info` | GET | Device information |
| `/api/mode` | GET | Current dashboard mode |

### Main Dashboard API

#### WiFi Management

| Endpoint | Method | Parameters | Response |
|----------|--------|------------|----------|
| `/api/wifi/scan` | GET | - | Starts WiFi scan |
| `/api/wifi/scan/results` | GET | - | Returns network list |
| `/api/wifi/connect` | POST | `ssid`, `password` | Connection result |
| `/api/wifi/disconnect` | POST | - | Disconnection result |

**Example Response:**
```json
{
  "success": true,
  "ssid": "HomeNetwork",
  "ip": "192.168.1.100",
  "rssi": -45,
  "message": "Connected successfully"
}
```

#### GSM Operations

| Endpoint | Method | Parameters | Response |
|----------|--------|------------|----------|
| `/api/gsm/signal` | GET | `force` (optional) | Signal strength |
| `/api/gsm/network` | GET | `force` (optional) | Network info |
| `/api/gsm/call` | POST | `phoneNumber` | Call status |
| `/api/gsm/call/hangup` | POST | - | Hangup result |
| `/api/gsm/sms` | POST | `phoneNumber`, `message` | SMS status |

**Signal Response:**
```json
{
  "ok": true,
  "dbm": -75,
  "csq": 19,
  "grade": "Good"
}
```

**Network Response:**
```json
{
  "carrierName": "Dialog",
  "networkMode": "LTE",
  "isRegistered": true
}
```

#### Configuration Management

| Endpoint | Method | Parameters | Description |
|----------|--------|------------|-------------|
| `/api/load/user` | GET | - | Load user profile |
| `/api/save/user` | POST | `name`, `email`, `phone` | Save user profile |
| `/api/load/gsm` | GET | - | Load GSM config |
| `/api/save/gsm` | POST | `carrierName`, `apn`, `apnUser`, `apnPass` | Save GSM config |

### Email Dashboard API

| Endpoint | Method | Parameters | Description |
|----------|--------|------------|-------------|
| `/api/load/email` | GET | - | Load email config |
| `/api/save/email` | POST | `smtpHost`, `smtpPort`, `emailAccount`, `emailPassword`, `senderName` | Save email config |
| `/api/email/gsm/send` | POST | `to`, `subject`, `content` | Send email via GSM |
| `/api/load/ap` | GET | - | Load AP config |
| `/api/save/ap` | POST | `apSsid`, `apPass` | Save AP config |

## 📂 File Structure

### Configuration Files (SPIFFS)

```
/spiffs/
├── wifi.json       # WiFi credentials (AP & STA)
├── gsm.json        # GSM/APN configuration
├── user.json       # User profile data
└── email.json      # SMTP email settings
```

**wifi.json structure:**
```json
{
  "staSsid": "HomeNetwork",
  "staPass": "password123",
  "apSsid": "ESP32-Config",
  "apPass": "12345678"
}
```

**gsm.json structure:**
```json
{
  "carrierName": "Dialog",
  "apn": "internet",
  "apnUser": "",
  "apnPass": ""
}
```

**email.json structure:**
```json
{
  "smtpHost": "smtp.gmail.com",
  "smtpPort": 465,
  "emailAccount": "your-email@gmail.com",
  "emailPassword": "app-password",
  "senderName": "ESP32 Device"
}
```

## 🔍 Troubleshooting

### Common Issues

#### 1. ESP32 Won't Connect to WiFi

**Symptoms:** Cannot access `192.168.4.1` after connecting to AP

**Solutions:**
- Verify AP password is at least 8 characters
- Check if multiple devices are connected (limit: varies by ESP32)
- Try restarting ESP32 and reconnecting
- Ensure device WiFi isn't stuck in STA mode (check Serial Monitor)

#### 2. GSM Modem Not Responding

**Symptoms:** No GSM signal, calls/SMS fail

**Solutions:**
```cpp
// Check power supply
// GSM modules need 2A capable power source
// Verify wiring:
GPIO 17 (TX)  →  GSM RX
GPIO 16 (RX)  ←  GSM TX
GND          →  GND

// Add capacitor near GSM VCC (100-1000µF)
// Ensure SIM card is properly inserted
// Check antenna connection
```

#### 3. Email Sending Fails

**Symptoms:** Email returns error or timeout

**Solutions:**
- **Gmail Users**: Enable 2-factor authentication and create App Password
  - Go to: https://myaccount.google.com/apppasswords
  - Generate new app password (not your Gmail password)
  - Use this 16-character password in email config
- Verify GSM has internet connectivity (check APN settings)
- Ensure SMTP port 465 is correct for your provider
- Test GSM signal strength (`/api/gsm/signal`)

#### 4. Double Reset Not Working

**Symptoms:** Mode doesn't switch after double reset

**Solutions:**
- Timing is critical: press reset twice within 3 seconds
- Watch Serial Monitor for "DOUBLE RESET DETECTED" message
- If timeout is too short, modify: `#define DRD_TIMEOUT 5000` (5 seconds)
- Verify SPIFFS is working (check Serial Monitor during boot)

#### 5. Captive Portal Not Appearing

**Symptoms:** Phone/laptop doesn't show "Sign in to network" popup

**Solutions:**
- Manually navigate to `http://192.168.4.1`
- Disable cellular data on phone (forces captive portal check)
- Try different browser (Chrome, Safari work best)
- iOS: Go to WiFi settings → tap (i) icon → forget network → reconnect

### Debug Mode

Enable verbose logging by modifying Serial prints:

```cpp
// In setup()
Serial.setDebugOutput(true);
WiFi.setOutputPower(20);  // Max power for debugging
```

View detailed logs in Serial Monitor at 115200 baud.

## 🛡️ Security Considerations

### Password Storage
- Passwords stored in SPIFFS are **not encrypted**
- For production use, implement encryption (e.g., AES)
- Change default AP password immediately

### Network Security
- Use WPA2 encryption for AP mode (automatic with 8+ character password)
- Consider implementing authentication for API endpoints
- Use HTTPS if connecting to external services

### Gmail App Passwords
- Never use your main Gmail password
- Always use app-specific passwords
- Revoke unused app passwords regularly

## ✨ Future Implimentation


1. **OTA implimentations.**
2. **Send Emails via WiFi.**
3. **Real sensor and actuator integration.**



## 📊 Operation Flow Diagrams

### Voice Call Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    GSM Voice Call Flow                      │
└─────────────────────────────────────────────────────────────┘

User Request (POST /api/gsm/call)
    │
    ├─► Validate phone number
    │
    ├─► Check SIM card status (AT+CPIN?)
    │   │
    │   ├─► Response: "READY" ✓
    │   └─► Response: Error ✗ → Return failure
    │
    ├─► Send dial command (ATD+94719792341;)
    │   │
    │   └─► Wait for "OK" response (10s timeout)
    │
    ├─► Call established ✓
    │   │
    │   └─► Keep active for 10 seconds
    │
    ├─► Auto-hangup (ATH)
    │   │
    │   └─► Wait for "OK" response
    │
    └─► Return success/failure to user

Typical Response Time: 2-3 seconds
Call Duration: 10 seconds (configurable)
Network Requirement: GSM/3G/4G registration
```

### SMS Sending Flow

```
┌─────────────────────────────────────────────────────────────┐
│                      GSM SMS Flow                           │
└─────────────────────────────────────────────────────────────┘

User Request (POST /api/gsm/sms)
    │
    ├─► Validate phone number and message
    │
    ├─► Check SIM card status (AT+CPIN?)
    │   │
    │   ├─► Response: "READY" ✓
    │   └─► Response: Error ✗ → Return failure
    │
    ├─► Set SMS text mode (AT+CMGF=1)
    │   │
    │   └─► Wait for "OK" response
    │
    ├─► Send SMS command (AT+CMGS="+94719792341")
    │   │
    │   └─► Wait for '>' prompt (5s timeout)
    │
    ├─► Send message content
    │   │
    │   └─► "Hello from ESP32!"
    │
    ├─► Send Ctrl+Z (ASCII 26) to finalize
    │   │
    │   └─► Wait for +CMGS response (15s timeout)
    │
    ├─► Check delivery confirmation
    │   │
    │   ├─► Response: "+CMGS: 123" + "OK" ✓
    │   └─► Response: Error ✗ → Return failure
    │
    └─► Return success/failure to user

Typical Response Time: 5-10 seconds
Message Limit: 160 characters (single SMS)
Network Requirement: GSM registration
```

### Email Sending Flow (GSM SMTP)

```
┌─────────────────────────────────────────────────────────────┐
│                 GSM Email (SMTP) Flow                       │
└─────────────────────────────────────────────────────────────┘

User Request (POST /api/email/gsm/send)
    │
    ├─► Validate email configuration
    │   │
    │   ├─► Check SMTP host, port
    │   ├─► Check email account
    │   └─► Check app password
    │
    ├─► Check SIM card status (AT+CPIN?)
    │   │
    │   ├─► Response: "READY" ✓
    │   └─► Response: Error ✗ → Return failure
    │
    ├─► Initialize GPRS connection
    │   │
    │   ├─► Set APN (AT+CSTT="internet")
    │   ├─► Bring up connection (AT+CIICR)
    │   └─► Get IP address (AT+CIFSR)
    │
    ├─► Establish SMTP connection
    │   │
    │   ├─► Connect to smtp.gmail.com:465
    │   ├─► Wait for "220" response
    │   └─► Send EHLO command
    │
    ├─► Authenticate with SMTP server
    │   │
    │   ├─► Send AUTH LOGIN
    │   ├─► Send base64(email account)
    │   └─► Send base64(app password)
    │
    ├─► Compose email
    │   │
    │   ├─► MAIL FROM: <sender@gmail.com>
    │   ├─► RCPT TO: <recipient@example.com>
    │   ├─► DATA command
    │   ├─► Send headers (From, To, Subject)
    │   ├─► Send body content
    │   └─► End with "." on new line
    │
    ├─► Wait for server confirmation
    │   │
    │   ├─► Response: "250 OK" ✓
    │   └─► Response: Error ✗ → Return failure
    │
    ├─► Close SMTP connection (QUIT)
    │
    ├─► Close GPRS connection (AT+CIPSHUT)
    │
    └─► Return success/failure to user

Typical Response Time: 15-30 seconds
Network Requirement: GPRS/3G/4G data connection
SMTP Protocol: SSL/TLS on port 465
Gmail Requirement: App-specific password (not account password)
```

### Complete Operation Comparison

```
┌──────────────┬─────────────┬─────────────┬──────────────────┐
│  Operation   │  Time Taken │  Network    │  SIM Credit      │
├──────────────┼─────────────┼─────────────┼──────────────────┤
│  Voice Call  │   2-3 sec   │  GSM/3G/4G  │  Per minute rate │
│  SMS Message │   5-10 sec  │  GSM        │  Per SMS rate    │
│  Email (GSM) │  15-30 sec  │  GPRS/Data  │  Data charges    │
└──────────────┴─────────────┴─────────────┴──────────────────┘
```

### Error Handling Flow

```
┌─────────────────────────────────────────────────────────────┐
│              Common Error Handling Flow                     │
└─────────────────────────────────────────────────────────────┘

Operation Start
    │
    ├─► Check SIM Status
    │   │
    │   ├─► SIM PIN Required → Error: "SIM requires PIN"
    │   ├─► SIM PUK Required → Error: "SIM blocked (PUK)"
    │   ├─► No SIM → Error: "SIM not inserted"
    │   └─► READY ✓ → Continue
    │
    ├─► Check Network Registration
    │   │
    │   ├─► Not Registered → Error: "No network"
    │   ├─► Registration Denied → Error: "Network denied"
    │   └─► Registered ✓ → Continue
    │
    ├─► Check Signal Strength
    │   │
    │   ├─► < -100 dBm → Warning: "Weak signal"
    │   └─► >= -100 dBm ✓ → Continue
    │
    ├─► Execute Operation
    │   │
    │   ├─► Timeout → Error: "Operation timeout"
    │   ├─► Invalid Response → Error: "Modem error"
    │   └─► Success ✓ → Return result
    │
    └─► Log Result and Return

Error Response Format:
{
  "success": false,
  "error": "Descriptive error message",
  "errorCode": "SIM_NOT_READY",
  "timestamp": 1234567890
}
```

### Network Connection States

```
┌─────────────────────────────────────────────────────────────┐
│            GSM Network Connection States                    │
└─────────────────────────────────────────────────────────────┘

State 0: Not Searching
    └─► Modem idle, no network search

State 1: Registered (Home Network) ✓
    └─► Ready for all operations (Call, SMS, Data)

State 2: Searching for Network
    └─► Wait for registration

State 3: Registration Denied ✗
    └─► Check SIM validity and network coverage

State 4: Unknown
    └─► Modem error or invalid state

State 5: Registered (Roaming) ⚠
    └─► Operations work but may incur roaming charges

Recommended State for Operations: 1 (Home) or 5 (Roaming)
```

## 🔄 Changelog

### Version 2.3.0 (2025-01-30)
- ✨ Added double reset detection for dashboard switching
- ✨ Implemented dual dashboard system (Main + Email)
- ✨ Added GSM email sending capabilities
- ✨ Enhanced WiFi network scanning with caching
- 🐛 Fixed AP password validation issues
- 📝 Improved documentation and API responses

### Version 2.2.0 (2025-01-15)
- ✨ Added sensor monitoring capabilities
- ✨ Implemented GSM call and SMS features
- 🔧 Optimized SPIFFS file handling

### Version 2.1.0 (2025-01-01)
- ✨ Initial release with WiFi and GSM support
- ✨ Basic configuration dashboard
- ✨ Captive portal functionality

---

**Made with ❤️ for the ESP32 community**
