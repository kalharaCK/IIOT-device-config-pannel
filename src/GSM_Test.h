/**
 * @file GSM_Test.h
 * @brief GSM Test Library for SIMCom A76xx modules
 * @author IoT Device Dashboard Project
 * @version 1.0.0
 * @date 2025-01-30
 * 
 * @details
 * This library provides a comprehensive interface for communicating with SIMCom A76xx
 * series GSM/LTE modems (A7600, A7600E, A7600F) using AT commands. It supports all
 * essential GSM operations including voice calls, SMS messaging, network information
 * retrieval, and signal strength monitoring.
 * 
 * @section features Features
 * - SIM card status checking and validation
 * - Voice call initiation and termination
 * - SMS message sending with delivery confirmation
 * - Network information detection (carrier, mode, registration status)
 * - Signal strength and quality monitoring
 * - Raw AT command interface for advanced operations
 * - Comprehensive error handling and timeout management
 * 
 * @section hardware Hardware Setup
 * - ESP32 DevKit board (any variant)
 * - SIMCom A76xx GSM/LTE modem (A7600/A7600E/A7600F)
 * - Wiring connections:
 *   - ESP32 RX2 (GPIO 16) -> Modem TX
 *   - ESP32 TX2 (GPIO 17) -> Modem RX
 *   - Common GND connection
 *   - Power supply (3.3V or 5V depending on modem)
 * 
 * @section usage Usage Example
 * @code
 * #include "GSM_Test.h"
 * 
 * GSM_Test gsm(Serial2, 16, 17, 115200);
 * 
 * void setup() {
 *   gsm.begin();
 *   if (gsm.checkSIM()) {
 *     Serial.println("SIM card ready");
 *     gsm.sendSMS("+1234567890", "Hello from ESP32!");
 *   }
 * }
 * @endcode
 * 
 * @section at_commands AT Commands Used
 * - AT+CPIN? - Check SIM card status
 * - ATD<number>; - Make voice call
 * - ATH - Hang up call
 * - AT+CMGF=1 - Set SMS text mode
 * - AT+CMGS="<number>" - Send SMS
 * - AT+CSQ - Get signal strength
 * - AT+COPS? - Get network operator
 * - AT+CREG? - Get network registration status
 * - AT+QNWINFO - Get network information
 * 
 * @section limitations Limitations
 * - SMS receiving not implemented (send-only)
 * - No data connection management
 * - No GPRS/HTTP functionality
 * - Limited to basic GSM operations
 * 
 * @section troubleshooting Troubleshooting
 * - Ensure proper wiring and power supply
 * - Check SIM card insertion and activation
 * - Verify network coverage and signal strength
 * - Monitor serial output for AT command responses
 * - Use raw AT command interface for debugging
 */

#ifndef GSM_TEST_H
#define GSM_TEST_H

// Standard Arduino library for hardware abstraction
#include <Arduino.h>

/**
 * @class GSM_Test
 * @brief Main class for GSM/LTE modem communication
 * 
 * This class provides a high-level interface for communicating with SIMCom A76xx
 * series GSM/LTE modems. It encapsulates all AT command operations and provides
 * convenient methods for common GSM operations.
 */
class GSM_Test {
public:
  // ============================================================================
  // CONSTRUCTOR AND INITIALIZATION
  // ============================================================================
  
  /**
   * @brief Constructor for GSM_Test class
   * @param modemSerial Reference to HardwareSerial instance (typically Serial2)
   * @param rxPin ESP32 RX pin connected to modem TX (default: 16)
   * @param txPin ESP32 TX pin connected to modem RX (default: 17)
   * @param baudRate Serial communication baud rate (default: 115200)
   * 
   * @note The constructor only stores the configuration parameters.
   * Call begin() after construction to initialize the serial communication.
   */
  GSM_Test(HardwareSerial& modemSerial, int rxPin = 16, int txPin = 17, long baudRate = 115200);
  
  /**
   * @brief Initialize GSM modem communication
   * 
   * This method initializes the serial communication with the GSM modem.
   * It should be called once in the setup() function after constructing the GSM_Test object.
   * 
   * @note This method includes a 2-second delay to allow the modem to initialize.
   * The method will print initialization information to the Serial monitor.
   */
  void begin();
  
  // ============================================================================
  // SIM CARD OPERATIONS
  // ============================================================================
  
  /**
   * @brief Check SIM card status and readiness
   * @return true if SIM card is ready and operational, false otherwise
   * 
   * This method sends the AT+CPIN? command to check if the SIM card is
   * properly inserted and ready for operations. A SIM card is considered
   * ready when it responds with "READY" status.
   * 
   * @note This method should be called before attempting any GSM operations
   * that require SIM card functionality (calls, SMS, network operations).
   */
  bool checkSIM();
  
  // ============================================================================
  // VOICE CALL OPERATIONS
  // ============================================================================
  
  /**
   * @brief Initiate a voice call to the specified phone number
   * @param phoneNumber Phone number in international format (e.g., "+94719792341")
   * @return true if call initiation was successful, false otherwise
   * 
   * This method sends the ATD command to initiate a voice call. The phone number
   * should be in international format starting with '+' followed by country code.
   * 
   * @note The method only initiates the call. The actual call connection depends
   * on network conditions and the recipient's availability.
   * 
   * @warning Ensure the SIM card is ready and has sufficient credit before calling.
   */
  bool makeCall(String phoneNumber);
  
  /**
   * @brief Terminate the current active call
   * @return true if hangup was successful, false otherwise
   * 
   * This method sends the ATH command to terminate any active voice call.
   * It can be used to end both outgoing and incoming calls.
   */
  bool hangupCall();
  
  // ============================================================================
  // SMS OPERATIONS
  // ============================================================================
  
  /**
   * @brief Send an SMS message to the specified phone number
   * @param phoneNumber Recipient phone number in international format
   * @param message SMS message text content
   * @return true if SMS was sent successfully, false otherwise
   * 
   * This method handles the complete SMS sending process:
   * 1. Sets SMS to text mode (AT+CMGF=1)
   * 2. Sends the recipient number (AT+CMGS="<number>")
   * 3. Waits for the '>' prompt
   * 4. Sends the message content
   * 5. Sends Ctrl+Z to finalize the SMS
   * 
   * @note The phone number should be in international format.
   * SMS messages are limited to 160 characters for single SMS.
   * 
   * @warning Ensure the SIM card is ready and has sufficient credit.
   */
  bool sendSMS(String phoneNumber, String message);
  
  // ============================================================================
  // RAW AT COMMAND INTERFACE
  // ============================================================================
  
  /**
   * @brief Send a raw AT command and retrieve the response
   * @param command AT command string to send (without CR/LF)
   * @param timeout Timeout in milliseconds for response (default: 5000)
   * @return Complete response string from the modem
   * 
   * This method provides direct access to the modem's AT command interface.
   * It can be used for advanced operations not covered by the high-level methods.
   * 
   * @note The command should not include carriage return or line feed characters.
   * The method automatically adds the proper termination.
   * 
   * @warning Use with caution as incorrect AT commands can affect modem operation.
   */
  String sendATCommand(String command, unsigned long timeout = 5000);
  
  // ============================================================================
  // NETWORK INFORMATION STRUCTURES AND METHODS
  // ============================================================================
  
  /**
   * @struct NetworkInfo
   * @brief Structure containing comprehensive network information
   * 
   * This structure holds all relevant network information retrieved from the GSM modem.
   * It provides a complete picture of the current network status and connection details.
   */
  struct NetworkInfo {
    String carrierName;        // Network operator name (e.g., "Dialog", "Mobitel")
    String mcc;               // Mobile Country Code (3-digit country identifier)
    String mnc;               // Mobile Network Code (2-3 digit network identifier)
    int signalStrength;       // Signal strength in dBm (-113 to -51)
    int signalQuality;        // Signal quality on 0-31 scale
    String networkMode;       // Network technology (GSM, LTE, 3G, etc.)
    bool isRegistered;        // Network registration status (true if registered)
    String locationAreaCode;  // Location Area Code (LAC) for cell identification
    String cellId;           // Cell ID for precise location identification
  };
  
  /**
   * @brief Automatically detect and retrieve comprehensive carrier network information
   * @return NetworkInfo structure containing all available network details
   * 
   * This method performs a comprehensive network information retrieval by sending
   * multiple AT commands to gather all available network data:
   * - AT+COPS? - Network operator information
   * - AT+CSQ - Signal strength and quality
   * - AT+CREG? - Network registration status
   * - AT+QNWINFO - Network mode information
   * 
   * @note This method may take several seconds to complete as it sends multiple
   * AT commands and waits for responses. The returned structure contains
   * "Unknown" values for any information that couldn't be retrieved.
   */
  NetworkInfo detectCarrierNetwork();
  
  /**
   * @brief Get current signal strength in dBm
   * @return Signal strength in dBm (-113 to -51), or -999 if error
   * 
   * This method sends the AT+CSQ command to retrieve the current signal strength.
   * The signal strength is converted from the modem's 0-31 scale to dBm values.
   * 
   * @note Signal strength interpretation:
   * - -51 dBm to -70 dBm: Excellent signal
   * - -70 dBm to -85 dBm: Good signal  
   * - -85 dBm to -100 dBm: Fair signal
   * - -100 dBm to -113 dBm: Poor signal
   * - -999: Error or no signal
   */
  int getSignalStrength();
  


  // ============================================================================
  // PRIVATE MEMBERS AND METHODS
  // ============================================================================
  
private:
  // Hardware configuration
  HardwareSerial& _modemSerial;  // Reference to the serial port used for communication
  int _rxPin;                    // ESP32 RX pin number
  int _txPin;                    // ESP32 TX pin number
  long _baudRate;                // Serial communication baud rate
  
  /**
   * @brief Wait for a specific response from the modem
   * @param expectedResponse The exact response string to wait for
   * @param timeout Timeout in milliseconds (default: 5000)
   * @return true if the expected response was received, false on timeout
   * 
   * This private method continuously reads from the serial port until either
   * the expected response is found or the timeout period expires. It's used
   * internally by other methods to wait for specific AT command responses.
   * 
   * @note The method performs case-sensitive string matching.
   */
  bool waitForResponse(String expectedResponse, unsigned long timeout = 5000);
  
  /**
   * @brief Wait for any response from the modem
   * @param timeout Timeout in milliseconds (default: 5000)
   * @return Complete response string received from the modem
   * 
   * This private method reads all available data from the serial port
   * until the timeout period expires. It's used to capture complete
   * AT command responses for parsing and analysis.
   * 
   * @note The returned string may contain multiple lines and control characters.
   */
  String waitForAnyResponse(unsigned long timeout = 5000);
};

#endif
