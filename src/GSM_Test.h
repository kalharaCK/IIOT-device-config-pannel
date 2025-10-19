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
/* ---------------------------------------------------------------------------
   SMTP.h
   Header for the SMTP helper class.

   Public API:
     - begin()
     - setAPN("apn")
     - setAuth("gmail", "appPassword")
     - setRecipient("to@domain", "Name")
     - setFromName("Name")
     - setSubject("Subject")
     - setBody("Body text")
     - sendEmail()      → handles TLS + SMTP session
     - bridge(Serial)   → passthrough for debugging
--------------------------------------------------------------------------- */

#ifndef SMTP_H
#define SMTP_H

#include <Arduino.h>

#ifndef SMTP_DEBUG
#define SMTP_DEBUG 1
#endif

class SMTP {
public:
  SMTP(HardwareSerial& modem, int rxPin, int txPin, long baud=115200);

  void begin();
  void setAPN(const char* apn);
  void setAuth(const char* gmail, const char* appPassword);
  void setRecipient(const char* to, const char* name="");
  void setFromName(const char* name);
  void setSubject(const String& subject);
  void setBody(const String& body);

  bool sendEmail();           // main function
  void bridge(Stream& usb);   // passthrough mode

private:
  HardwareSerial& _m;
  int _rxPin, _txPin;
  long _baud;
  String _apn, _gmail, _appPass, _to, _toName, _fromName, _subject, _body;

  // Internal helpers
  bool AT(const String& cmd, const String& expect="OK", uint32_t ms=10000);
  bool ATAcceptAny(const String& cmd, const String* tokens, size_t ntokens, uint32_t ms=10000);
  bool waitFor(const String& token, uint32_t ms=10000);
  bool readUntil(String& out, const String& token, uint32_t ms=10000);

  bool bringUpPDP();
  void tearDownPDP();
  bool cchStart();
  bool cchOpen(const char* host, uint16_t port, int link=0);
  bool cchSendRaw(int link, const uint8_t* data, size_t len);
  bool cchSendLine(int link, const String& line);
  bool cchRecvChunk(int link, String& out, uint16_t maxBytes=512, uint32_t ms=4000);
  void cchClose(int link);
  void cchStop();
  String b64(const String& in);
  bool smtpExpect(int link, const char* code, uint32_t ms=10000);
  bool smtpSession(int link);
};

#endif
