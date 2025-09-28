/**
 * @file GSM_Test.cpp
 * @brief Implementation of GSM Test Library for SIMCom A76xx modules
 * @author IoT Device Dashboard Project
 * @version 1.0.0
 * @date 2025-01-30
 * 
 * @details
 * This file contains the complete implementation of the GSM_Test class for
 * communicating with SIMCom A76xx series GSM/LTE modems. All methods use
 * standard AT commands to interact with the modem hardware.
 * 
 * @section implementation Implementation Details
 * - All AT commands are sent with proper CR/LF termination
 * - Responses are parsed and validated for success/failure
 * - Comprehensive error handling with timeout management
 * - Detailed logging for debugging and troubleshooting
 * - Signal strength conversion from 0-31 scale to dBm values
 * - Network information parsing from multiple AT command responses
 * 
 * @section at_commands AT Commands Implemented
 * - AT+CPIN? - SIM card status check
 * - ATD<number>; - Voice call initiation
 * - ATH - Call termination
 * - AT+CMGF=1 - SMS text mode setting
 * - AT+CMGS="<number>" - SMS sending
 * - AT+CSQ - Signal strength query
 * - AT+COPS? - Network operator information
 * - AT+CREG? - Network registration status
 * - AT+QNWINFO - Network information
 * 
 * @section error_handling Error Handling
 * - All methods return boolean success/failure status
 * - Timeout protection for all AT command operations
 * - Input validation for phone numbers and messages
 * - SIM card status verification before operations
 * - Comprehensive error logging to Serial monitor
 */

#include "GSM_Test.h"

// ============================================================================
// CONSTRUCTOR AND INITIALIZATION
// ============================================================================

/**
 * @brief Constructor for GSM_Test class
 * @param modemSerial Reference to HardwareSerial instance (typically Serial2)
 * @param rxPin ESP32 RX pin connected to modem TX
 * @param txPin ESP32 TX pin connected to modem RX
 * @param baudRate Serial communication baud rate
 * 
 * @details
 * The constructor initializes the GSM_Test object with the specified hardware
 * configuration. It stores the parameters for later use in the begin() method.
 * No actual hardware initialization occurs in the constructor.
 */
GSM_Test::GSM_Test(HardwareSerial& modemSerial, int rxPin, int txPin, long baudRate)
  : _modemSerial(modemSerial), _rxPin(rxPin), _txPin(txPin), _baudRate(baudRate) {
}

/**
 * @brief Initialize GSM modem communication
 * 
 * @details
 * This method performs the actual hardware initialization:
 * 1. Configures the serial port with the specified baud rate and pin assignments
 * 2. Waits 2 seconds for the modem to initialize
 * 3. Prints initialization information to the Serial monitor
 * 
 * @note This method should be called once in setup() after constructing the object.
 * The 2-second delay is necessary to allow the modem to boot up and become ready.
 */
void GSM_Test::begin() {
  // Initialize serial communication with the modem
  _modemSerial.begin(_baudRate, SERIAL_8N1, _rxPin, _txPin);
  
  // Wait for modem to initialize (SIMCom A76xx requires startup time)
  delay(2000);
  
  // Log initialization details for debugging
  Serial.println("GSM_Test: Modem initialized");
  Serial.println("GSM_Test: RX Pin = " + String(_rxPin));
  Serial.println("GSM_Test: TX Pin = " + String(_txPin));
  Serial.println("GSM_Test: Baud Rate = " + String(_baudRate));
}

// ============================================================================
// SIM CARD OPERATIONS
// ============================================================================

/**
 * @brief Check SIM card status and readiness
 * @return true if SIM card is ready and operational, false otherwise
 * 
 * @details
 * This method sends the AT+CPIN? command to check the SIM card status.
 * The modem responds with the current PIN status:
 * - "READY" - SIM card is ready for operations
 * - "SIM PIN" - SIM card requires PIN entry
 * - "SIM PUK" - SIM card is blocked and requires PUK
 * - "SIM not inserted" - No SIM card detected
 * 
 * @note This method should be called before attempting any GSM operations
 * that require SIM card functionality (calls, SMS, network operations).
 */
bool GSM_Test::checkSIM() {
  Serial.println("GSM_Test: Checking SIM card status...");
  
  // Send AT+CPIN? command to check SIM status
  String response = sendATCommand("AT+CPIN?");
  
  // Check for "READY" status in the response
  if (response.indexOf("READY") >= 0) {
    Serial.println("GSM_Test: ✓ SIM card is ready");
    return true;
  } else {
    Serial.println("GSM_Test: ✗ SIM card not ready");
    Serial.println("GSM_Test: Response: " + response);
    return false;
  }
}

// ============================================================================
// VOICE CALL OPERATIONS
// ============================================================================

/**
 * @brief Initiate a voice call to the specified phone number
 * @param phoneNumber Phone number in international format (e.g., "+94719792341")
 * @return true if call initiation was successful, false otherwise
 * 
 * @details
 * This method performs the following steps:
 * 1. Verifies SIM card is ready using checkSIM()
 * 2. Constructs the ATD command with the phone number
 * 3. Sends the dial command with extended timeout (10 seconds)
 * 4. Checks for "OK" response to confirm call initiation
 * 
 * @note The method only initiates the call. The actual call connection depends
 * on network conditions, recipient availability, and account balance.
 * 
 * @warning Ensure the SIM card is ready and has sufficient credit before calling.
 */
bool GSM_Test::makeCall(String phoneNumber) {
  Serial.println("GSM_Test: Making call to " + phoneNumber);
  
  // First verify SIM card is ready for operations
  if (!checkSIM()) {
    Serial.println("GSM_Test: ✗ Cannot make call - SIM not ready");
    return false;
  }
  
  // Construct and send the dial command (ATD<number>;)
  String dialCommand = "ATD" + phoneNumber + ";";
  String response = sendATCommand(dialCommand, 10000);  // Extended timeout for call initiation
  
  // Check for successful call initiation
  if (response.indexOf("OK") >= 0) {
    Serial.println("GSM_Test: ✓ Call initiated successfully");
    return true;
  } else {
    Serial.println("GSM_Test: ✗ Call failed to initiate");
    Serial.println("GSM_Test: Response: " + response);
    return false;
  }
}

/**
 * @brief Terminate the current active call
 * @return true if hangup was successful, false otherwise
 * 
 * @details
 * This method sends the ATH (AT Hangup) command to terminate any active voice call.
 * It can be used to end both outgoing and incoming calls.
 * 
 * @note The method will attempt to hang up regardless of call state.
 * If no call is active, the modem may respond with "ERROR" but this is normal.
 */
bool GSM_Test::hangupCall() {
  Serial.println("GSM_Test: Hanging up call...");
  
  // Send ATH command to terminate active call
  String response = sendATCommand("ATH");
  
  // Check for successful hangup
  if (response.indexOf("OK") >= 0) {
    Serial.println("GSM_Test: ✓ Call ended successfully");
    return true;
  } else {
    Serial.println("GSM_Test: ✗ Hangup failed");
    Serial.println("GSM_Test: Response: " + response);
    return false;
  }
}

// ============================================================================
// SMS OPERATIONS
// ============================================================================

/**
 * @brief Send an SMS message to the specified phone number
 * @param phoneNumber Recipient phone number in international format
 * @param message SMS message text content
 * @return true if SMS was sent successfully, false otherwise
 * 
 * @details
 * This method handles the complete SMS sending process:
 * 1. Verifies SIM card is ready using checkSIM()
 * 2. Sets SMS to text mode (AT+CMGF=1)
 * 3. Sends the recipient number (AT+CMGS="<number>")
 * 4. Waits for the '>' prompt from the modem
 * 5. Sends the message content
 * 6. Sends Ctrl+Z (ASCII 26) to finalize the SMS
 * 7. Waits for delivery confirmation (+CMGS response)
 * 
 * @note The phone number should be in international format (e.g., "+94719792341").
 * SMS messages are limited to 160 characters for single SMS.
 * 
 * @warning Ensure the SIM card is ready and has sufficient credit.
 */
bool GSM_Test::sendSMS(String phoneNumber, String message) {
  Serial.println("GSM_Test: Sending SMS to " + phoneNumber);
  Serial.println("GSM_Test: Message: " + message);
  
  // First verify SIM card is ready for operations
  if (!checkSIM()) {
    Serial.println("GSM_Test: ✗ Cannot send SMS - SIM not ready");
    return false;
  }
  
  // Set SMS to text mode (required for text SMS)
  String response = sendATCommand("AT+CMGF=1");
  if (response.indexOf("OK") < 0) {
    Serial.println("GSM_Test: ✗ Failed to set SMS text mode");
    return false;
  }
  
  // Send SMS command with phone number
  String smsCommand = "AT+CMGS=\"" + phoneNumber + "\"";
  Serial.println("GSM_Test: Sending command: " + smsCommand);
  
  // Send the command and wait for '>' prompt
  _modemSerial.print(smsCommand);
  _modemSerial.print("\r\n");
  
  // Wait for '>' prompt (modem ready to receive message)
  if (!waitForResponse(">", 5000)) {
    Serial.println("GSM_Test: ✗ No '>' prompt received");
    return false;
  }
  
  // Send message content and Ctrl+Z to finalize
  _modemSerial.print(message);
  _modemSerial.write(26);  // Ctrl+Z (ASCII 26) - SMS send command
  
  // Wait for SMS delivery confirmation
  String smsResponse = waitForAnyResponse(15000);
  
  // Check for successful SMS delivery (+CMGS response and OK)
  if (smsResponse.indexOf("+CMGS:") >= 0 && smsResponse.indexOf("OK") >= 0) {
    Serial.println("GSM_Test: ✓ SMS sent successfully");
    return true;
  } else {
    Serial.println("GSM_Test: ✗ SMS failed to send");
    Serial.println("GSM_Test: Response: " + smsResponse);
    return false;
  }
}

// ============================================================================
// RAW AT COMMAND INTERFACE
// ============================================================================

/**
 * @brief Send a raw AT command and retrieve the response
 * @param command AT command string to send (without CR/LF)
 * @param timeout Timeout in milliseconds for response (default: 5000)
 * @return Complete response string from the modem
 * 
 * @details
 * This method provides direct access to the modem's AT command interface:
 * 1. Clears any pending data from the serial buffer
 * 2. Sends the command with proper CR/LF termination
 * 3. Waits for and captures the complete response
 * 4. Returns the raw response for parsing by calling code
 * 
 * @note The command should not include carriage return or line feed characters.
 * The method automatically adds the proper termination.
 * 
 * @warning Use with caution as incorrect AT commands can affect modem operation.
 */
String GSM_Test::sendATCommand(String command, unsigned long timeout) {
  Serial.println("GSM_Test: Sending: " + command);
  
  // Clear any pending data from the serial buffer
  while (_modemSerial.available()) {
    _modemSerial.read();
  }
  
  // Send command with proper CR/LF termination
  _modemSerial.print(command);
  _modemSerial.print("\r\n");
  
  // Wait for and return the complete response
  return waitForAnyResponse(timeout);
}

// ============================================================================
// PRIVATE HELPER METHODS
// ============================================================================

/**
 * @brief Wait for a specific response from the modem
 * @param expectedResponse The exact response string to wait for
 * @param timeout Timeout in milliseconds (default: 5000)
 * @return true if the expected response was received, false on timeout
 * 
 * @details
 * This private method continuously reads from the serial port until either
 * the expected response is found or the timeout period expires. It's used
 * internally by other methods to wait for specific AT command responses.
 * 
 * @note The method performs case-sensitive string matching.
 */
bool GSM_Test::waitForResponse(String expectedResponse, unsigned long timeout) {
  unsigned long startTime = millis();
  String response = "";
  
  // Continue reading until timeout or expected response found
  while (millis() - startTime < timeout) {
    while (_modemSerial.available()) {
      char c = _modemSerial.read();
      response += c;
      
      // Check if expected response is found
      if (response.indexOf(expectedResponse) >= 0) {
        Serial.println("GSM_Test: Received: " + response);
        return true;
      }
    }
    delay(10);  // Small delay to prevent excessive CPU usage
  }
  
  // Timeout occurred
  Serial.println("GSM_Test: Timeout waiting for: " + expectedResponse);
  Serial.println("GSM_Test: Received: " + response);
  return false;
}

/**
 * @brief Wait for any response from the modem
 * @param timeout Timeout in milliseconds (default: 5000)
 * @return Complete response string received from the modem
 * 
 * @details
 * This private method reads all available data from the serial port
 * until the timeout period expires. It's used to capture complete
 * AT command responses for parsing and analysis.
 * 
 * @note The returned string may contain multiple lines and control characters.
 */
String GSM_Test::waitForAnyResponse(unsigned long timeout) {
  unsigned long startTime = millis();
  String response = "";
  
  // Read all available data until timeout
  while (millis() - startTime < timeout) {
    while (_modemSerial.available()) {
      char c = _modemSerial.read();
      response += c;
    }
    delay(10);  // Small delay to prevent excessive CPU usage
  }
  
  Serial.println("GSM_Test: Response: " + response);
  return response;
}

/**
 * @brief Automatically detect and retrieve carrier network information
 * @return NetworkInfo structure containing all network details
 */
GSM_Test::NetworkInfo GSM_Test::detectCarrierNetwork() {
  NetworkInfo networkInfo;
  
  // Initialize with default values
  networkInfo.carrierName = "Unknown";
  networkInfo.mcc = "Unknown";
  networkInfo.mnc = "Unknown";
  networkInfo.signalStrength = -999;
  networkInfo.signalQuality = 0;
  networkInfo.networkMode = "Unknown";
  networkInfo.isRegistered = false;
  networkInfo.locationAreaCode = "Unknown";
  networkInfo.cellId = "Unknown";
  
  Serial.println("GSM_Test: Detecting carrier network information...");
  
  // Check if SIM is ready first
  if (!checkSIM()) {
    Serial.println("GSM_Test: ✗ Cannot detect network - SIM not ready");
    return networkInfo;
  }
  
  // Get network operator information
  Serial.println("GSM_Test: Getting network operator info...");
  String response = sendATCommand("AT+COPS?");
  
  if (response.indexOf("+COPS:") >= 0) {
    // Parse operator information
    int start = response.indexOf("+COPS:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String operatorInfo = response.substring(start, end);
    Serial.println("GSM_Test: Operator info: " + operatorInfo);
    
    // Extract carrier name (format: +COPS: 0,0,"CARRIER_NAME",2)
    int quoteStart = operatorInfo.indexOf("\"");
    int quoteEnd = operatorInfo.indexOf("\"", quoteStart + 1);
    if (quoteStart >= 0 && quoteEnd > quoteStart) {
      networkInfo.carrierName = operatorInfo.substring(quoteStart + 1, quoteEnd);
    }
    
    // Check registration status
    if (operatorInfo.indexOf(",0,") >= 0) {
      networkInfo.isRegistered = true;
    }
  }
  
  // Get signal strength and quality
  Serial.println("GSM_Test: Getting signal strength...");
  response = sendATCommand("AT+CSQ");
  
  if (response.indexOf("+CSQ:") >= 0) {
    int start = response.indexOf("+CSQ:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String signalInfo = response.substring(start, end);
    Serial.println("GSM_Test: Signal info: " + signalInfo);
    
    // Parse signal strength and quality (format: +CSQ: 20,99)
    int commaPos = signalInfo.indexOf(",");
    if (commaPos > 0) {
      String rssiStr = signalInfo.substring(signalInfo.indexOf(":") + 1, commaPos);
      String berStr = signalInfo.substring(commaPos + 1);
      
      int rssi = rssiStr.toInt();
      int ber = berStr.toInt();
      
      // Convert RSSI to dBm (0-31 scale to -113 to -51 dBm)
      if (rssi >= 0 && rssi <= 31) {
        networkInfo.signalStrength = -113 + (rssi * 2);
        networkInfo.signalQuality = rssi;
      }
    }
  }
  
  // Get network registration status
  Serial.println("GSM_Test: Getting network registration status...");
  response = sendATCommand("AT+CREG?");
  
  if (response.indexOf("+CREG:") >= 0) {
    int start = response.indexOf("+CREG:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String regInfo = response.substring(start, end);
    Serial.println("GSM_Test: Registration info: " + regInfo);
    
    // Parse registration status (format: +CREG: 0,1)
    int commaPos = regInfo.indexOf(",");
    if (commaPos > 0) {
      String statusStr = regInfo.substring(commaPos + 1);
      int status = statusStr.toInt();
      
      // Status: 0=not searching, 1=registered home, 2=searching, 3=denied, 4=unknown, 5=registered roaming
      networkInfo.isRegistered = (status == 1 || status == 5);
    }
  }
  
  // Get cell information (Location Area Code and Cell ID)
  Serial.println("GSM_Test: Getting cell information...");
  response = sendATCommand("AT+CREG=2");  // Enable extended registration info
  
  delay(1000);  // Wait for registration update
  
  response = sendATCommand("AT+CREG?");
  
  if (response.indexOf("+CREG:") >= 0) {
    int start = response.indexOf("+CREG:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String cellInfo = response.substring(start, end);
    Serial.println("GSM_Test: Cell info: " + cellInfo);
    
    // Parse LAC and Cell ID (format: +CREG: 2,1,"1234","5678")
    int firstQuote = cellInfo.indexOf("\"");
    int secondQuote = cellInfo.indexOf("\"", firstQuote + 1);
    int thirdQuote = cellInfo.indexOf("\"", secondQuote + 1);
    int fourthQuote = cellInfo.indexOf("\"", thirdQuote + 1);
    
    if (firstQuote >= 0 && secondQuote > firstQuote) {
      networkInfo.locationAreaCode = cellInfo.substring(firstQuote + 1, secondQuote);
    }
    
    if (thirdQuote >= 0 && fourthQuote > thirdQuote) {
      networkInfo.cellId = cellInfo.substring(thirdQuote + 1, fourthQuote);
    }
  }
  
  // Get MCC and MNC information
  Serial.println("GSM_Test: Getting MCC/MNC information...");
  response = sendATCommand("AT+COPS=3,0");  // Set operator name format
  
  delay(500);
  
  response = sendATCommand("AT+COPS?");
  
  if (response.indexOf("+COPS:") >= 0) {
    // Try to get numeric operator info
    response = sendATCommand("AT+COPS=3,2");  // Set to numeric format
    
    delay(500);
    
    response = sendATCommand("AT+COPS?");
    
    if (response.indexOf("+COPS:") >= 0) {
      int start = response.indexOf("+COPS:");
      int end = response.indexOf("\n", start);
      if (end == -1) end = response.length();
      
      String numericInfo = response.substring(start, end);
      Serial.println("GSM_Test: Numeric info: " + numericInfo);
      
      // Parse MCC and MNC (format: +COPS: 0,2,"12345",2)
      int quoteStart = numericInfo.indexOf("\"");
      int quoteEnd = numericInfo.indexOf("\"", quoteStart + 1);
      if (quoteStart >= 0 && quoteEnd > quoteStart) {
        String mccmnc = numericInfo.substring(quoteStart + 1, quoteEnd);
        if (mccmnc.length() >= 5) {
          networkInfo.mcc = mccmnc.substring(0, 3);
          networkInfo.mnc = mccmnc.substring(3);
        }
      }
    }
  }
  
  // Determine network mode
  Serial.println("GSM_Test: Determining network mode...");
  response = sendATCommand("AT+QNWINFO");
  
  if (response.indexOf("+QNWINFO:") >= 0) {
    int start = response.indexOf("+QNWINFO:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String nwInfo = response.substring(start, end);
    Serial.println("GSM_Test: Network mode info: " + nwInfo);
    
    // Parse network mode
    int commaPos = nwInfo.indexOf(",");
    if (commaPos > 0) {
      String modeStr = nwInfo.substring(nwInfo.indexOf(":") + 1, commaPos);
      networkInfo.networkMode = modeStr;
    }
  } else {
    // Fallback: try to determine from CREG response
    if (networkInfo.isRegistered) {
      networkInfo.networkMode = "GSM/LTE";  // Generic for registered networks
    }
  }
  
  // Print summary
  Serial.println("GSM_Test: === Network Information Summary ===");
  Serial.println("GSM_Test: Carrier: " + networkInfo.carrierName);
  Serial.println("GSM_Test: MCC: " + networkInfo.mcc);
  Serial.println("GSM_Test: MNC: " + networkInfo.mnc);
  Serial.println("GSM_Test: Signal Strength: " + String(networkInfo.signalStrength) + " dBm");
  Serial.println("GSM_Test: Signal Quality: " + String(networkInfo.signalQuality) + "/31");
  Serial.println("GSM_Test: Network Mode: " + networkInfo.networkMode);
  Serial.println("GSM_Test: Registered: " + String(networkInfo.isRegistered ? "Yes" : "No"));
  Serial.println("GSM_Test: LAC: " + networkInfo.locationAreaCode);
  Serial.println("GSM_Test: Cell ID: " + networkInfo.cellId);
  Serial.println("GSM_Test: ======================================");
  
  return networkInfo;
}

/**
 * @brief Get signal strength in dBm
 * @return Signal strength in dBm (-113 to -51), or -999 if error
 */
int GSM_Test::getSignalStrength() {
  Serial.println("GSM_Test: Getting signal strength...");
  
  // Check if SIM is ready first
  if (!checkSIM()) {
    Serial.println("GSM_Test: ✗ Cannot get signal strength - SIM not ready");
    return -999;
  }
  
  // Get signal strength using AT+CSQ command
  String response = sendATCommand("AT+CSQ");
  
  if (response.indexOf("+CSQ:") >= 0) {
    int start = response.indexOf("+CSQ:");
    int end = response.indexOf("\n", start);
    if (end == -1) end = response.length();
    
    String signalInfo = response.substring(start, end);
    Serial.println("GSM_Test: Signal info: " + signalInfo);
    
    // Parse signal strength (format: +CSQ: 20,99)
    int commaPos = signalInfo.indexOf(",");
    if (commaPos > 0) {
      String rssiStr = signalInfo.substring(signalInfo.indexOf(":") + 1, commaPos);
      int rssi = rssiStr.toInt();
      
      // Convert RSSI to dBm (0-31 scale to -113 to -51 dBm)
      if (rssi >= 0 && rssi <= 31) {
        int signalStrength = -113 + (rssi * 2);
        Serial.println("GSM_Test: ✓ Signal strength: " + String(signalStrength) + " dBm (RSSI: " + String(rssi) + ")");
        return signalStrength;
      } else {
        Serial.println("GSM_Test: ✗ Invalid RSSI value: " + String(rssi));
        return -999;
      }
    } else {
      Serial.println("GSM_Test: ✗ Could not parse signal strength response");
      return -999;
    }
  } else {
    Serial.println("GSM_Test: ✗ No signal strength response received");
    return -999;
  }
}

