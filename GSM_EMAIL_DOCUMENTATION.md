# GSM Email Implementation Documentation

## Overview

This implementation adds GSM email functionality to the ESP32 IoT Configuration Panel, complementing the existing WiFi email system. Both email systems use the same SMTP credentials for consistency, but route through different network connections.

## Features

- **Dual Email Support**: WiFi and GSM email with shared configuration
- **APN Configuration**: Default APN set to "internet" as requested
- **Same SMTP Credentials**: Uses identical email settings for both WiFi and GSM
- **SIMCom A76xx Support**: Optimized for A7600, A7600E, A7600F modules
- **SSL/TLS Support**: Secure email transmission
- **Web API Integration**: RESTful endpoints for email management
- **Real-time Status Monitoring**: Integration with existing dashboard

## Architecture

### Email Flow Comparison

```
WiFi Email Flow:
ESP32 -> WiFi Connection -> Internet -> SMTP Server (smtp.gmail.com:465)

GSM Email Flow:
ESP32 -> GSM Modem -> Cellular Data (APN="internet") -> Internet -> SMTP Server (smtp.gmail.com:465)
```

### Implementation Components

1. **GSM_Test Library Extensions** (`GSM_Test.h/cpp`)
   - Added GSM SMTP client initialization
   - APN configuration support
   - Email sending methods using SIMCom A76xx built-in SMTP

2. **Main Application Integration** (`main.cpp`)
   - Separated WiFi and GSM email functions
   - New API endpoints for GSM email operations
   - Shared configuration management

3. **SMTP Library Integration** (`SMTP.h/cpp`)
   - Existing GSM-focused SMTP implementation
   - SSL/TLS support for secure email transmission
   - PDP context management for data connections

## API Endpoints

### GSM Email Endpoints

| Endpoint              | Method | Description        |
|----------             |--------|-------------       |
| `/api/email/gsm/send` | POST   | Send email via GSM |

**Request Format:**
```json
{
  "to": "recipient@example.com",
  "subject": "Test Email via GSM",
  "content": "Email body content"
}
```

**Response Format:**
```json
{
  "success": true,
  "message": "GSM email sent successfully",
  "method": "GSM"
}
```

### Existing WiFi Email Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/email/send` | POST | Send email via WiFi |
| `/api/load/email` | GET | Load email configuration |
| `/api/save/email` | POST | Save email configuration |

## Configuration

### GSM SMTP Configuration

The GSM email system uses the same configuration as WiFi email:

```cpp
// Email configuration shared between WiFi and GSM
struct EmailConfig {
  String smtpHost;        // Default: "smtp.gmail.com"
  int smtpPort;           // Default: 465 (SSL)
  String emailAccount;    // Gmail address
  String emailPassword;   // Gmail App Password
  String senderName;      // Display name
};
```

### APN Configuration

The GSM email system uses APN="internet" by default:

```cpp
// Initialize GSM SMTP with APN
gsmModem.initSMTP("internet");

// APN is automatically set
gsmModem.configSMTP(smtpHost, smtpPort, emailAccount, appPassword, senderName);
```

## Usage Examples

### Programming Interface

```cpp
#include "GSM_Test.h"

GSM_Test gsmModem(Serial2, 16, 17, 115200);

void setup() {
  gsmModem.begin();
  
  // Initialize GSM SMTP with APN="internet"
  gsmModem.initSMTP("internet");
  
  // Configure with same credentials as WiFi email
  gsvmModem.configSMTP("smtp.gmail.com", 465, 
                       "your-email@gmail.com", 
                       "your-app-password", 
                       "GSM Device");
}

void sendTestEmail() {
  bool success = gsmModem.sendEmailViaGSM(
    "recipient@example.com",
    "Test Email via GSM",
    "This email was sent via GSM data connection!"
  );
  
  if (success) {
    Serial.println("GSM email sent successfully!");
  }
}
```

### Web Dashboard Integration

The web dashboard now includes options for both WiFi and GSM email sending:

1. Configure email settings once (shared between WiFi and GSM)
2. Choose email method: WiFi or GSM
3. Test email functionality via web interface

## Hardware Requirements

### ESP32 Configuration
- ESP32 DevKit (any variant)
- Hardware Serial 2 (GPIO 16/17)

### GSM Modem
- SIMCom A76xx series (A7600/A7600E/A7600F)
- Active SIM card with data plan
- APN support ("internet" preferred)

### Wiring Connections
```
ESP32          GSM Modem
------         ---------
GPIO 16 (RX2) -> TX
GPIO 17 (TX2) -> RX
GND           -> GND
VCC           -> VCC (3.3V or 5V depending on modem)
```

## Setup Instructions

### 1. Hardware Setup
1. Connect ESP32 to GSM modem according to wiring diagram
2. Insert active SIM card with data plan
3. Apply power to both ESP32 and GSM modem

### 2. Software Configuration
1. Upload the updated firmware to ESP32
2. Access the web dashboard at `http://192.168.4.1`
3. Configure email settings in the Email Settings tab
4. Ensure GSM network registration

### 3. Email Configuration
1. **Gmail Setup**:
   - Enable 2-Factor Authentication
   - Generate App Password for "Mail"
   - Use App Password, not regular password

2. **SMTP Settings**:
   - Host: `smtp.gmail.com`
   - Port: `465` (SSL)
   - Username: Your Gmail address
   - Password: Gmail App Password

### 4. GSM Configuration
1. Verify SIM card insertion and activation
2. Check network registration status
3. Ensure APN is set to "internet" (automatic)
4. Test GSM connectivity before sending emails

## Troubleshooting

### Common Issues

1. **SIM Card Not Ready**
   ```
   GSM_Test: ✗ SIM card not ready
   ```
   - Check SIM card insertion
   - Verify SIM card activation
   - Check network coverage

2. **GSM SMTP Initialization Failed**
   ```
   GSM_Test: ✗ Failed to initialize GSM SMTP client
   ```
   - Ensure GSM network registration
   - Check signal strength
   - Verify APN configuration

3. **Email Sending Failed**
   ```
   GSM_Test: ✗ Email failed to send via GSM
   ```
   - Check SMTP credentials
   - Verify GSM data connection
   - Ensure sufficient signal strength

4. **SMTP Authentication Failed**
   ```
   GSM_Test: ✗ SMTP authentication failed
   ```
   - Use Gmail App Password (not regular password)
   - Check 2FA settings
   - Verify email account credentials

### Debug Commands

Use the GSM_Test library debug methods:

```cpp
// Check SIM status
gsmModem.checkSIM();

// Check signal strength
int signal = gsmModem.getSignalStrength();

// Get network information
GSM_Test::NetworkInfo network = gsmModem.detectCarrierNetwork();

// Send raw AT command for debugging
String response = gsmModem.sendATCommand("AT+CREG?");
```

## Testing Procedures

### 1. GSM Connectivity Test
1. Check GSM network registration
2. Verify signal strength (>-100 dBm recommended)
3. Test data connectivity (APN functionality)

### 2. Email Functionality Test
1. Initialize GSM SMTP client
2. Configure email credentials
3. Send test email via GSM
4. Verify email delivery

### 3. Dual-Mode Testing
1. Test WiFi email functionality
2. Test GSM email functionality
3. Compare response times and reliability
4. Verify shared configuration works for both modes

## Performance Considerations

### GSM Email Characteristics
- **Latency**: Higher than WiFi (cellular network dependent)
- **Reliability**: Depends on cellular signal strength
- **Cost**: Uses cellular data (check data plan)
- **Power**: Higher power consumption during transmission

### Optimization Tips
1. Cache GSM SMTP initialization
2. Batch multiple emails if possible
3. Implement retry logic for failed emails
4. Monitor battery usage during GSM operations

## Future Enhancements

1. **Email Queue Management**: Store emails for retry if GSM connection fails
2. **Signal Quality Monitoring**: Automatic fallback between WiFi and GSM
3. **Power Optimization**: Sleep modes during idle periods
4. **SMS Integration**: Combine SMS and Email notifications
5. **Multi-carrier Support**: Carrier-specific APN configurations

## Integration Notes

### Web Dashboard Updates
The web dashboard automatically detects the new GSM email functionality and provides:
- Email method selection (WiFi/GSM)
- Status indicators for both connection types
- Unified email configuration interface

### Mobile App Integration
The RESTful API enables mobile app integration:
- Cross-platform compatibility
- Real-time email status updates
- Remote configuration management

### IoT Platform Integration
The dual-mode email system supports:
- Cloud platform notifications
- Device status reporting
- Remote configuration updates
- Alert escalation systems

## Conclusion

The GSM email implementation successfully adds cellular email functionality to the ESP32 IoT Configuration Panel while maintaining compatibility with existing WiFi email features. Both systems share the same SMTP credentials and configuration, providing flexibility in email delivery methods based on network availability and requirements.

The implementation uses APN="internet" as requested and integrates seamlessly with the existing architecture, ensuring minimal disruption to current functionality while adding robust GSM email capabilities.

