/**
 * @file main.cpp
 * @brief ESP32 IoT Configuration Panel with GSM/LTE and Email Integration
 * @author IoT Device Dashboard Project
 * @version 2.2.0
 * @date 2025-01-30
 * 
 * Main application for ESP32-based IoT monitoring and configuration system.
 * Features:
 * - Dual-mode WiFi (Access Point + Station)
 * - GSM/LTE modem integration via SIMCom A76xx modules
 * - Email notification system with Gmail SMTP
 * - Web-based configuration dashboard with captive portal
 * - Real-time status monitoring and device management
 * - JSON-based configuration persistence
 * 
 * Hardware Requirements:
 * - IIoT dev module
 * - Wiring: ESP32 RX2 (GPIO 16) -> Modem TX, ESP32 TX2 (GPIO 17) -> Modem RX
 * 
 * Network Configuration:
 * - Access Point: ESP32-AccessPoint (default) or user-configured
 * - Station mode: Connects to user-specified WiFi networks
 * - Captive portal for easy initial configuration
 * 
 * API Endpoints:
 * - GET/POST /api/status - System status information
 * - GET/POST /api/wifi/* - WiFi management (scan, connect, disconnect)
 * - GET/POST /api/gsm/* - GSM operations (signal, network, SMS, calls)
 * - GET/POST /api/email/* - Email operations (send test emails)
 * - GET/POST /api/load/save/user - User profile management
 * - GET/POST /api/load/save/gsm - GSM configuration management
 * - GET/POST /api/load/save/email - Email configuration management
 */

// Standard ESP32 libraries
#include <Arduino.h>    // Core Arduino functionality
#include <WiFi.h>        // WiFi functionality for AP and Station modes
#include <WebServer.h>   // HTTP server for web interface
#include <DNSServer.h>   // DNS server for captive portal functionality
#include <SPIFFS.h>      // File system for configuration storage
#include <ArduinoJson.h> // JSON parsing and generation

#include <ESP_Mail_Client.h> // Email sending library

// Custom GSM library for SIMCom A76xx modules
#include "GSM_Test.h"

// Embedded web dashboard (stored in PROGMEM to save RAM)
#include "dashboard_html.h"   // Complete HTML/CSS/JS dashboard interface
#include "config_html.h"     // Configuration mode interface for DRD

// ============================================================================
// NETWORK AND SERVER CONFIGURATION
// ============================================================================

/**
 * @brief DNS server for captive portal functionality
 * Captive portal redirects all DNS queries to the ESP32's web interface
 * This allows devices to automatically open the configuration page when connecting
 */
#define DNS_PORT 53
DNSServer dnsServer;

/**
 * @brief HTTP web server instance
 * Handles all web requests and API endpoints on port 80
 */
WebServer server(80);

/**
 * @brief SMTP email client instance
 * Handles email sending via Gmail SMTP server
 */
SMTPSession smtp;

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

/**
 * @brief Default Access Point configuration
 * These values are used when no custom configuration is saved
 */
static const char* DEFAULT_AP_SSID = "ESP32-AccessPoint";  // Default AP name
static const char* DEFAULT_AP_PASS = "12345678";           // Default AP password (>= 8 chars)

/**
 * @brief Default Email configuration
 * These values are used when no custom email configuration is saved
 */
static const char* DEFAULT_SMTP_HOST = "smtp.gmail.com";   // Default SMTP server
static const int DEFAULT_SMTP_PORT = 465;                  // Default SMTP port (SSL)
static const char* DEFAULT_SENDER_NAME = "ESP32 Dashboard"; // Default sender name

/**
 * @brief Configuration file paths in SPIFFS
 * All configuration data is stored as JSON files in the SPIFFS filesystem
 */
static const char* WIFI_FILE = "/wifi.json";   // WiFi configuration (AP and Station settings)
static const char* GSM_FILE  = "/gsm.json";    // GSM modem configuration (APN, carrier settings)
static const char* USER_FILE = "/user.json";   // User profile data (name, email, phone)
static const char* EMAIL_FILE = "/email.json"; // Email configuration (SMTP settings)

// ============================================================================
// DRD (DEVICE RESET DETECTION) CONFIGURATION
// ============================================================================

/**
 * @brief DRD button configuration
 * Button connected between 5V and GPIO26 for device reset detection
 */
#define DRD_BUTTON_PIN 26        // GPIO pin for DRD button
#define DRD_DEBOUNCE_TIME 50     // Debounce time in milliseconds
#define DRD_DOUBLE_PRESS_TIME 5000 // Time window for double press detection (5 seconds)
#define DRD_HOLD_TIME 2000       // Time for long press detection (2 seconds)

/**
 * @brief DRD button states for state machine
 */
enum DRDButtonState {
  DRD_IDLE,           // Button not pressed
  DRD_PRESSED,        // Button pressed, waiting for debounce
  DRD_HELD,           // Button held for long press
  DRD_RELEASED        // Button released, waiting for debounce
};

/**
 * @brief DRD state variables with enhanced debouncing
 * These variables track the button press state and timing for robust detection
 */
volatile DRDButtonState drdButtonState = DRD_IDLE;     // Current button state
volatile unsigned long drdStateChangeTime = 0;          // Time of last state change
volatile unsigned long drdPressStartTime = 0;          // Time when press started
volatile unsigned long drdLastPressTime = 0;           // Timestamp of last complete press
volatile int drdPressCount = 0;                        // Number of presses in current window
volatile bool drdConfigMode = false;                   // Current mode: false=dashboard, true=config
volatile bool drdLongPressDetected = false;            // Flag for long press detection

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

/**
 * @brief GSM modem serial communication pins
 * ESP32 uses Hardware Serial 2 for communication with the GSM module
 */
static const int MODEM_RX = 16;  // ESP32 RX2 pin (connects to modem TX)
static const int MODEM_TX = 17;  // ESP32 TX2 pin (connects to modem RX)

/**
 * @brief GSM modem instance
 * Initialized with Serial2, pin configuration, and 115200 baud rate
 * This instance handles all AT command communication with the SIMCom A76xx module
 */
GSM_Test gsmModem(Serial2, MODEM_RX, MODEM_TX, 115200);

// ============================================================================
// GSM DATA CACHING SYSTEM
// ============================================================================

/**
 * @brief GSM data cache structure
 * 
 * This structure implements a caching system to reduce the frequency of AT commands
 * sent to the GSM modem. GSM operations can be slow and frequent queries can
 * impact system performance. The cache stores GSM data for 5 minutes and only
 * updates when explicitly requested or when the cache expires.
 * 
 * Benefits:
 * - Reduces AT command frequency (improves performance)
 * - Provides faster response times for dashboard updates
 * - Reduces power consumption
 * - Prevents modem overload
 */
struct GSMCache {
  // Signal strength and quality data
  int signalStrength = -999;    // Signal strength in dBm (-113 to -51)
  int signalQuality = 99;       // Signal quality (0-31 scale)
  String grade = "Unknown";     // Human-readable signal grade (Excellent/Good/Fair/Poor)
  
  // Network information
  String carrierName = "Unknown";   // Network operator name (e.g., "Dialog", "Mobitel")
  String networkMode = "Unknown";   // Network technology (GSM, LTE, etc.)
  bool isRegistered = false;        // Network registration status
  
  // Cache management
  unsigned long lastUpdate = 0;     // Timestamp of last cache update
  const unsigned long UPDATE_INTERVAL = 300000; // 5 minutes in milliseconds
  
  /**
   * @brief Check if cache needs updating
   * @param forceRefresh Force update regardless of cache age
   * @return true if cache should be updated
   */
  bool needsUpdate(bool forceRefresh = false) {
    return forceRefresh || (millis() - lastUpdate) > UPDATE_INTERVAL;
  }
  
  /**
   * @brief Update signal strength and quality data
   * @param forceRefresh Force update regardless of cache age
   * 
   * Retrieves current signal strength from the GSM modem and calculates
   * signal quality and grade. Updates are cached for 5 minutes to reduce
   * AT command frequency.
   */
  void updateSignal(bool forceRefresh = false) {
    if (needsUpdate(forceRefresh)) {
      signalStrength = gsmModem.getSignalStrength();
      if (signalStrength != -999) {
        // Convert dBm to 0-31 quality scale
        signalQuality = (signalStrength + 113) / 2;
        if (signalQuality < 0) signalQuality = 0;
        if (signalQuality > 31) signalQuality = 31;
        
        // Assign human-readable grade based on quality
        if (signalQuality >= 20) grade = "Excellent";
        else if (signalQuality >= 15) grade = "Good";
        else if (signalQuality >= 10) grade = "Fair";
        else grade = "Poor";
      }
      lastUpdate = millis();
    }
  }
  
  /**
   * @brief Update network information data
   * @param forceRefresh Force update regardless of cache age
   * 
   * Retrieves current network information including carrier name,
   * network mode, and registration status. Updates are cached for 5 minutes.
   */
  void updateNetwork(bool forceRefresh = false) {
    if (needsUpdate(forceRefresh)) {
      GSM_Test::NetworkInfo networkInfo = gsmModem.detectCarrierNetwork();
      carrierName = networkInfo.carrierName;
      networkMode = networkInfo.networkMode;
      isRegistered = networkInfo.isRegistered;
      lastUpdate = millis();
    }
  }
} gsmCache;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Convert IPAddress to String format
 * @param ip IPAddress object to convert
 * @return String representation of IP address (e.g., "192.168.1.100")
 */
String ipToStr(const IPAddress &ip) {
  char buf[24];
  snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  return String(buf);
}

/**
 * @brief Add CORS headers to HTTP response
 * 
 * Cross-Origin Resource Sharing (CORS) headers allow the web dashboard
 * to make API requests from different origins. This is essential for
 * the embedded web interface to communicate with the ESP32's API endpoints.
 */
void addCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");      // Allow all origins
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type"); // Allow content-type header
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS"); // Allow HTTP methods
  server.sendHeader("Cache-Control", "no-store");            // Prevent caching of responses
}

/**
 * @brief Send JSON response with CORS headers
 * @param code HTTP status code (200, 400, 500, etc.)
 * @param body JSON string to send as response body
 */
void sendJson(int code, const String& body) {
  addCORS();
  server.send(code, "application/json", body);
}

/**
 * @brief Send text response with CORS headers
 * @param code HTTP status code
 * @param body Text content to send as response body
 * @param ctype Content type (default: "text/plain")
 */
void sendText(int code, const String& body, const String& ctype = "text/plain") {
  addCORS();
  server.send(code, ctype, body);
}

// ============================================================================
// CONFIGURATION MANAGEMENT STRUCTURES
// ============================================================================

/**
 * @brief WiFi configuration management structure
 * 
 * Handles storage and retrieval of WiFi settings including:
 * - Station mode settings (connecting to existing WiFi networks)
 * - Access Point mode settings (ESP32 as WiFi hotspot)
 * 
 * Configuration is stored as JSON in SPIFFS filesystem for persistence
 * across reboots and power cycles.
 */
struct WifiConfig {
  // Station mode configuration (ESP32 connects to existing WiFi)
  String staSsid;  // WiFi network name to connect to
  String staPass;  // WiFi network password
  
  // Access Point mode configuration (ESP32 creates WiFi hotspot)
  String apSsid;   // Access Point name
  String apPass;   // Access Point password

  /**
   * @brief Load WiFi configuration from SPIFFS
   * @return true if configuration loaded successfully, false otherwise
   * 
   * Attempts to read WiFi configuration from /wifi.json file.
   * If file doesn't exist or is corrupted, returns false and uses defaults.
   */
  bool load() {
    if (!SPIFFS.exists(WIFI_FILE)) return false;
    File f = SPIFFS.open(WIFI_FILE, "r"); 
    if (!f) return false;
    
    DynamicJsonDocument doc(1024);
    auto err = deserializeJson(doc, f); 
    f.close();
    if (err) return false;
    
    // Load configuration with defaults
    staSsid = doc["staSsid"] | "";
    staPass = doc["staPass"] | "";
    apSsid  = doc["apSsid"]  | DEFAULT_AP_SSID;
    apPass  = doc["apPass"]  | DEFAULT_AP_PASS;
    return true;
  }
  
  /**
   * @brief Save WiFi configuration to SPIFFS
   * @return true if configuration saved successfully, false otherwise
   * 
   * Writes current WiFi configuration to /wifi.json file.
   * Uses default values if custom values are not set.
   */
  bool save() const {
    DynamicJsonDocument doc(1024);
    doc["staSsid"] = staSsid;
    doc["staPass"] = staPass;
    doc["apSsid"]  = apSsid.length() ? apSsid : DEFAULT_AP_SSID;
    doc["apPass"]  = apPass.length() ? apPass : DEFAULT_AP_PASS;
    
    File f = SPIFFS.open(WIFI_FILE, "w"); 
    if (!f) return false;
    serializeJson(doc, f); 
    f.close(); 
    return true;
  }
} wifiCfg;

/**
 * @brief GSM configuration management structure
 * 
 * Handles storage and retrieval of GSM modem settings including:
 * - Carrier/network operator information
 * - APN (Access Point Name) configuration for data connectivity
 * - APN authentication credentials
 * 
 * Configuration is stored as JSON in SPIFFS filesystem for persistence.
 */
struct GsmConfig {
  String carrierName;  // Network operator name (e.g., "Dialog", "Mobitel")
  String apn;          // Access Point Name for data connectivity
  String apnUser;      // APN username (if required)
  String apnPass;      // APN password (if required)
  
  /**
   * @brief Load GSM configuration from SPIFFS
   * @return true if configuration loaded successfully, false otherwise
   * 
   * Attempts to read GSM configuration from /gsm.json file.
   * If file doesn't exist or is corrupted, returns false and uses defaults.
   */
  bool load() {
    if (!SPIFFS.exists(GSM_FILE)) return false;
    File f = SPIFFS.open(GSM_FILE, "r"); 
    if (!f) return false;
    
    DynamicJsonDocument doc(1024); 
    if (deserializeJson(doc, f)) { 
      f.close(); 
      return false; 
    }
    f.close();
    
    // Load configuration with defaults
    carrierName = doc["carrierName"] | ""; 
    apn = doc["apn"] | "";
    apnUser = doc["apnUser"] | ""; 
    apnPass = doc["apnPass"] | "";
    return true;
  }
  
  /**
   * @brief Save GSM configuration to SPIFFS
   * @return true if configuration saved successfully, false otherwise
   * 
   * Writes current GSM configuration to /gsm.json file.
   */
  bool save() const {
    DynamicJsonDocument doc(1024);
    doc["carrierName"] = carrierName; 
    doc["apn"] = apn;
    doc["apnUser"] = apnUser; 
    doc["apnPass"] = apnPass;
    
    File f = SPIFFS.open(GSM_FILE, "w"); 
    if (!f) return false;
    serializeJson(doc, f); 
    f.close(); 
    return true;
  }
} gsmCfg;

/**
 * @brief User profile configuration management structure
 * 
 * Handles storage and retrieval of user profile information including:
 * - User's full name
 * - Email address for notifications
 * - Phone number for SMS testing and notifications
 * 
 * Configuration is stored as JSON in SPIFFS filesystem for persistence.
 */
struct UserConfig {
  String name;   // User's full name
  String email;  // User's email address
  String phone;  // User's phone number (international format)
  
  /**
   * @brief Load user configuration from SPIFFS
   * @return true if configuration loaded successfully, false otherwise
   * 
   * Attempts to read user configuration from /user.json file.
   * If file doesn't exist or is corrupted, returns false and uses defaults.
   */
  bool load() {
    if (!SPIFFS.exists(USER_FILE)) return false;
    File f = SPIFFS.open(USER_FILE, "r"); 
    if (!f) return false;
    
    DynamicJsonDocument doc(1024); 
    if (deserializeJson(doc, f)) { 
      f.close(); 
      return false; 
    }
    f.close();
    
    // Load configuration with defaults
    name = doc["name"] | ""; 
    email = doc["email"] | ""; 
    phone = doc["phone"] | "";
    return true;
  }
  
  /**
   * @brief Save user configuration to SPIFFS
   * @return true if configuration saved successfully, false otherwise
   * 
   * Writes current user configuration to /user.json file.
   */
  bool save() const {
    DynamicJsonDocument doc(1024);
    doc["name"] = name; 
    doc["email"] = email; 
    doc["phone"] = phone;
    
    File f = SPIFFS.open(USER_FILE, "w"); 
    if (!f) return false;
    serializeJson(doc, f); 
    f.close(); 
    return true;
  }
} userCfg;

/**
 * @brief Email configuration management structure
 * 
 * Handles storage and retrieval of email settings including:
 * - SMTP server host and port
 * - Email account credentials
 * - Sender name and email address
 * 
 * Configuration is stored as JSON in SPIFFS filesystem for persistence.
 */
struct EmailConfig {
  String smtpHost;        // SMTP server host (e.g., "smtp.gmail.com")
  int smtpPort;           // SMTP server port (e.g., 465 for SSL)
  String emailAccount;    // Email address for sending
  String emailPassword;   // App password for email account
  String senderName;      // Display name for sender
  
  /**
   * @brief Load email configuration from SPIFFS
   * @return true if configuration loaded successfully, false otherwise
   * 
   * Attempts to read email configuration from /email.json file.
   * If file doesn't exist or is corrupted, returns false and uses defaults.
   */
  bool load() {
    if (!SPIFFS.exists(EMAIL_FILE)) return false;
    File f = SPIFFS.open(EMAIL_FILE, "r"); 
    if (!f) return false;
    
    DynamicJsonDocument doc(1024); 
    if (deserializeJson(doc, f)) { 
      f.close(); 
      return false; 
    }
    f.close();
    
    // Load configuration with defaults
    smtpHost = doc["smtpHost"] | DEFAULT_SMTP_HOST;
    smtpPort = doc["smtpPort"] | DEFAULT_SMTP_PORT;
    emailAccount = doc["emailAccount"] | "";
    emailPassword = doc["emailPassword"] | "";
    senderName = doc["senderName"] | DEFAULT_SENDER_NAME;
    return true;
  }
  
  /**
   * @brief Save email configuration to SPIFFS
   * @return true if configuration saved successfully, false otherwise
   * 
   * Writes current email configuration to /email.json file.
   */
  bool save() const {
    DynamicJsonDocument doc(1024);
    doc["smtpHost"] = smtpHost;
    doc["smtpPort"] = smtpPort;
    doc["emailAccount"] = emailAccount;
    doc["emailPassword"] = emailPassword;
    doc["senderName"] = senderName;
    
    File f = SPIFFS.open(EMAIL_FILE, "w"); 
    if (!f) return false;
    serializeJson(doc, f); 
    f.close(); 
    return true;
  }
  
  /**
   * @brief Check if email configuration is valid
   * @return true if all required fields are populated
   */
  bool isValid() const {
    return smtpHost.length() > 0 && emailAccount.length() > 0 && emailPassword.length() > 0;
  }
} emailCfg;

// ============================================================================
// WIFI MANAGEMENT FUNCTIONS
// ============================================================================

/**
 * @brief Start Access Point mode with captive portal support
 * @param ssid Access Point SSID name
 * @param pass Access Point password (minimum 8 characters)
 * 
 * @details
 * This function configures the ESP32 to operate in dual mode (AP + STA):
 * 1. Sets WiFi mode to WIFI_AP_STA (Access Point + Station)
 * 2. Configures AP with static IP (192.168.4.1) and subnet mask
 * 3. Starts the Access Point with the specified credentials
 * 4. Falls back to default credentials if startup fails
 * 5. Starts DNS server for captive portal functionality
 * 
 * @note The function includes a 500ms delay to allow the AP to initialize.
 * The captive portal DNS server redirects all DNS queries to the ESP32's IP.
 */
void startAP(const String& ssid, const String& pass) {
  // Set WiFi to dual mode (Access Point + Station)
  WiFi.mode(WIFI_AP_STA);
  
  // Configure AP with static IP settings for better reliability
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  
  // Start Access Point with provided credentials (password must be >= 8 chars)
  bool ap_ok = WiFi.softAP(ssid.c_str(), pass.length() >= 8 ? pass.c_str() : NULL);
  if (!ap_ok) {
    // Fallback to default credentials if startup fails
    WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
  }
  
  delay(500); // Give AP time to initialize and become stable
  
  // Start DNS server for captive portal functionality
  if (!dnsServer.start(DNS_PORT, "*", WiFi.softAPIP())) {
    Serial.println("DNS Server failed to start");
  } else {
    Serial.println("DNS Server started for captive portal");
  }
}

/**
 * @brief Connect to a WiFi network in Station mode
 * @param ssid WiFi network name to connect to
 * @param pass WiFi network password
 * 
 * @details
 * This function initiates a connection to the specified WiFi network.
 * It only attempts connection if a valid SSID is provided.
 * The connection process is non-blocking and continues in the background.
 * 
 * @note Use WiFi.status() to check connection status after calling this function.
 */
void connectSTA(const String& ssid, const String& pass) {
  if (!ssid.length()) return; // Skip if no SSID provided
  WiFi.begin(ssid.c_str(), pass.c_str());
}

/**
 * @brief Convert RSSI value to human-readable strength description
 * @param rssi Received Signal Strength Indicator in dBm
 * @return String description of signal strength
 * 
 * @details
 * Converts RSSI values to descriptive strength levels:
 * - -60 dBm and above: "strong" (excellent signal)
 * - -60 to -75 dBm: "medium" (good signal)
 * - Below -75 dBm: "weak" (poor signal)
 * 
 * @note RSSI values are negative numbers, with higher (less negative) values
 * indicating stronger signal strength.
 */
const char* rssiToStrength(int rssi) {
  if (rssi >= -60) return "strong";
  if (rssi >= -75) return "medium";
  return "weak";
}

// ============================================================================
// EMAIL MANAGEMENT FUNCTIONS
// ============================================================================

/**
 * @brief Send email using WiFi SMTP settings
 * @param toEmail Recipient email address
 * @param subject Email subject line
 * @param content Email body content
 * @return true if email sent successfully, false otherwise
 * 
 * @details
 * This function sends an email using the ESP Mail Client library (WiFi).
 * It uses the configured SMTP settings and handles connection errors.
 * 
 * @note For Gmail, you need to use an App Password instead of your regular password.
 * Enable 2-factor authentication and generate an App Password in your Google account.
 */
bool sendEmailWiFi(const String& toEmail, const String& subject, const String& content) {
  // Check if email configuration is valid
  if (!emailCfg.isValid()) {
    Serial.println("WiFi Email configuration incomplete");
    return false;
  }

  // Configure SMTP session
  ESP_Mail_Session session;
  session.server.host_name = emailCfg.smtpHost.c_str();
  session.server.port = emailCfg.smtpPort;
  session.login.email = emailCfg.emailAccount.c_str();
  session.login.password = emailCfg.emailPassword.c_str();
  session.login.user_domain = "";

  // Configure email message
  SMTP_Message mail;
  mail.sender.name = emailCfg.senderName.c_str();
  mail.sender.email = emailCfg.emailAccount.c_str();
  mail.subject = subject.c_str();
  mail.addRecipient("Recipient", toEmail.c_str());
  mail.text.content = content.c_str();

  // Attempt to send email
  Serial.println("Attempting to send email via WiFi...");
  
  if (!smtp.connect(&session)) {
    Serial.println("Could not connect to SMTP server (WiFi)");
    return false;
  }
  
  if (!MailClient.sendMail(&smtp, &mail)) {
    Serial.println("Email sending failed (WiFi)");
    return false;
  }
  
  Serial.println("Email sent successfully via WiFi!");
  return true;
}

/**
 * @brief Send email using GSM SMTP settings
 * @param toEmail Recipient email address
 * @param subject Email subject line
 * @param content Email body content
 * @return true if email sent successfully, false otherwise
 * 
 * @details
 * This function sends an email using the GSM modem's built-in SMTP client.
 * It uses the same credentials as WiFi email but routes through GSM data connection.
 * Requires APN="internet" configuration.
 */
bool sendEmailGSM(const String& toEmail, const String& subject, const String& content) {
  Serial.println("Attempting to send email via GSM...");
  
  // Check if email configuration is valid
  if (!emailCfg.isValid()) {
    Serial.println("GSM Email configuration incomplete");
    return false;
  }
  
  // Initialize GSM SMTP client with APN="internet"
  if (!gsmModem.initSMTP("internet")) {
    Serial.println("Failed to initialize GSM SMTP client");
    return false;
  }
  
  // Configure GSM SMTP with same credentials as WiFi
  if (!gsmModem.configSMTP(emailCfg.smtpHost.c_str(), 
                          emailCfg.smtpPort, 
                          emailCfg.emailAccount.c_str(), 
                          emailCfg.emailPassword.c_str(), 
                          emailCfg.senderName.c_str())) {
    Serial.println("Failed to configure GSM SMTP");
    return false;
  }
  
  // Send email via GSM
  bool success = gsmModem.sendEmailViaGSM(toEmail, subject, content);
  
  if (success) {
    Serial.println("Email sent successfully via GSM!");
  } else {
    Serial.println("Email sending failed via GSM");
  }
  
  return success;
}

// ============================================================================
// JSON RESPONSE BUILDERS
// ============================================================================

/**
 * @brief Build comprehensive system status JSON response
 * @return JSON string containing complete system status information
 * 
 * @details
 * This function creates a comprehensive JSON response containing:
 * - System mode (AP+STA)
 * - Access Point information (SSID, IP, MAC, connected devices)
 * - Station mode information (SSID, connection status, IP, RSSI, hostname)
 * - Connection status with appropriate CSS classes for UI styling
 * 
 * The response is used by the web dashboard to display real-time system status.
 * All values are dynamically retrieved from the current WiFi state.
 */
String buildStatusJson() {
  DynamicJsonDocument doc(1024);
  doc["mode"] = "AP+STA";

  // Access Point information
  JsonObject ap = doc.createNestedObject("ap");
  ap["ssid"] = wifiCfg.apSsid.length() ? wifiCfg.apSsid : DEFAULT_AP_SSID;
  ap["ip"]   = ipToStr(WiFi.softAPIP());
  ap["mac"]  = WiFi.softAPmacAddress();
  ap["connectedDevices"] = WiFi.softAPgetStationNum(); // Number of connected devices

  // Station mode information
  JsonObject sta = doc.createNestedObject("sta");
  bool staConnected = (WiFi.status() == WL_CONNECTED);
  sta["ssid"]      = staConnected ? WiFi.SSID() : "";
  sta["connected"] = staConnected;
  sta["ip"]        = staConnected ? ipToStr(WiFi.localIP()) : "0.0.0.0";
  sta["rssi"]      = staConnected ? WiFi.RSSI() : 0;
  sta["hostname"]  = WiFi.getHostname() ? WiFi.getHostname() : "";

  // Status message and CSS class for UI styling
  if (staConnected) { 
    String statusMsg = "Connected to ";
    statusMsg += WiFi.SSID();
    sta["status"] = statusMsg;
    sta["statusClass"] = "status-connected"; 
  } else { 
    sta["status"] = "Not connected"; 
    sta["statusClass"] = "status-disconnected"; 
  }

  // Email configuration status
  JsonObject email = doc.createNestedObject("email");
  email["configured"] = emailCfg.isValid();
  email["account"] = emailCfg.emailAccount;

  String out; 
  serializeJson(doc, out); 
  return out;
}

// ============================================================================
// GSM SIGNAL PARSING UTILITIES
// ============================================================================

/**
 * @struct CSQInfo
 * @brief Structure to hold parsed CSQ (Signal Quality) information
 * 
 * This structure contains the parsed results from AT+CSQ command responses.
 * CSQ provides signal strength and quality information from the GSM modem.
 */
struct CSQInfo {
  int csq = 99;   // Signal quality (0-31 scale, 99 = unknown)
  int ber = 99;   // Bit Error Rate (0-7 scale, 99 = unknown)
  int dbm = 0;    // Signal strength in dBm (calculated from CSQ)
  const char* label = "Unknown"; // Human-readable signal quality description
};

/**
 * @brief Parse CSQ (Signal Quality) response from GSM modem
 * @param line Raw response line from AT+CSQ command
 * @return CSQInfo structure with parsed signal information
 * 
 * @details
 * This function parses the response from AT+CSQ command which typically
 * returns a string like "+CSQ: 20,99" where:
 * - First number (20) is the signal quality (0-31)
 * - Second number (99) is the bit error rate (0-7, 99=unknown)
 * 
 * The function converts CSQ to dBm using the formula: dBm = -113 + (2 * CSQ)
 * and assigns human-readable quality labels based on CSQ values.
 * 
 * @note CSQ values: 0-10=Poor, 11-15=Fair, 16-20=Good, 21-31=Excellent
 */
CSQInfo parseCSQ(const String& line) {
  CSQInfo o;
  
  // Find the colon and comma positions in the response
  int colon = line.indexOf(':');
  int comma = line.indexOf(',', colon + 1);
  
  // Return default values if parsing fails
  if (colon < 0 || comma < 0) return o;
  
  // Extract and parse the two numeric values
  String a = line.substring(colon + 1, comma); a.trim();
  String b = line.substring(comma + 1);        b.trim();
  o.csq = a.toInt(); 
  o.ber = b.toInt();
  
  // Convert CSQ to dBm and assign quality label
  if (o.csq >= 0 && o.csq <= 31) {
    o.dbm = -113 + (2 * o.csq);
    
    // Assign human-readable quality labels
    if (o.csq >= 20) o.label = "Excellent";
    else if (o.csq >= 15) o.label = "Good";
    else if (o.csq >= 10) o.label = "Fair";
    else o.label = "Poor";
  }
  
  return o;
}

// ============================================================================
// HTTP REQUEST HANDLERS
// ============================================================================

/**
 * @brief Handle root path requests (/) with captive portal support
 * 
 * @details
 * This function handles requests to the root path and implements captive portal
 * functionality. It detects captive portal detection requests from various
 * devices and operating systems and redirects them to the main dashboard.
 * 
 * Captive portal detection is supported for:
 * - Apple devices (captive.apple.com)
 * - Windows devices (msftconnecttest.com, msftncsi.com)
 * - Android devices (connectivitycheck.gstatic.com)
 * - Firefox browser (detectportal.firefox.com)
 * 
 * For all other requests, it serves the main dashboard HTML page.
 */
void handleRoot() {
  String host = server.hostHeader();
  String uri = server.uri();
  
  // Check for captive portal detection requests
  if (host.startsWith("connectivitycheck.") || 
      host.startsWith("captive.apple.com") ||
      host.startsWith("msftconnecttest.") || 
      host.startsWith("detectportal.") ||
      host.startsWith("www.msftconnecttest.com") ||
      host.startsWith("www.msftncsi.com") ||
      host.startsWith("connectivitycheck.gstatic.com") ||
      host.startsWith("detectportal.firefox.com")) {
    
    // Send proper captive portal redirect response
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
    return;
  }
  
  // Serve different interfaces based on DRD mode
  addCORS();
  if (drdConfigMode) {
    // Serve configuration mode interface
    Serial.println("Serving Configuration Mode interface");
    server.send_P(200, "text/html", config_html, config_html_len);
  } else {
    // Serve normal dashboard interface
    Serial.println("Serving Dashboard Mode interface");
    server.send_P(200, "text/html", dashboard_html, dashboard_html_len);
  }
}

/**
 * @brief Handle HTTP OPTIONS requests for CORS preflight
 * 
 * @details
 * This function handles CORS preflight requests (OPTIONS method) that browsers
 * send before making cross-origin requests. It responds with appropriate
 * CORS headers and a 204 No Content status.
 */
void handleOptions() { 
  addCORS(); 
  server.send(204); 
}

/**
 * @brief Handle 404 Not Found requests with captive portal support
 * 
 * @details
 * This function handles requests to non-existent paths. It implements the same
 * captive portal detection logic as handleRoot() to ensure that captive portal
 * detection requests are properly redirected even when they hit non-existent
 * paths.
 * 
 * For other 404 requests, it serves the main dashboard page.
 */
void handleNotFound() { 
  // Check for captive portal detection requests
  String host = server.hostHeader();
  if (host.startsWith("connectivitycheck.") || 
      host.startsWith("captive.apple.com") ||
      host.startsWith("msftconnecttest.") || 
      host.startsWith("detectportal.") ||
      host.startsWith("www.msftconnecttest.com") ||
      host.startsWith("www.msftncsi.com") ||
      host.startsWith("connectivitycheck.gstatic.com") ||
      host.startsWith("detectportal.firefox.com")) {
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
  } else {
    // For other 404s, serve the main page
    handleRoot(); 
  }
}

// ============================================================================
// EMAIL API HANDLERS
// ============================================================================

/**
 * @brief Handle email configuration load requests
 */
void handleEmailLoad() {
  DynamicJsonDocument doc(1024);
  doc["smtpHost"] = emailCfg.smtpHost;
  doc["smtpPort"] = emailCfg.smtpPort;
  doc["emailAccount"] = emailCfg.emailAccount;
  doc["senderName"] = emailCfg.senderName;
  // Note: Password is not returned for security reasons
  
  String out; 
  serializeJson(doc, out);
  sendJson(200, out);
}

/**
 * @brief Handle email configuration save requests
 */
void handleEmailSave() {
  // Validate request body
  if (!server.hasArg("plain")) { 
    sendText(400, "Invalid JSON"); 
    return; 
  }
  
  // Parse JSON request
  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, server.arg("plain"))) { 
    sendText(400, "Invalid JSON"); 
    return; 
  }
  
  // Update email configuration from request
  emailCfg.smtpHost = doc["smtpHost"] | DEFAULT_SMTP_HOST;
  emailCfg.smtpPort = doc["smtpPort"] | DEFAULT_SMTP_PORT;
  emailCfg.emailAccount = doc["emailAccount"] | "";
  emailCfg.emailPassword = doc["emailPassword"] | "";
  emailCfg.senderName = doc["senderName"] | DEFAULT_SENDER_NAME;
  
  // Save configuration to SPIFFS
  bool ok = emailCfg.save();

  // Log email configuration save
  Serial.print("Email configuration saved: ");
  Serial.print(emailCfg.emailAccount);
  Serial.print(", ");
  Serial.println(emailCfg.smtpHost);

  // Return response
  DynamicJsonDocument resp(256);
  resp["success"] = ok;
  if (ok) {
    resp["message"] = "Email configuration saved successfully";
  } else {
    resp["message"] = "Failed to save email configuration";
  }
  
  String out; 
  serializeJson(resp, out);
  sendJson(ok ? 200 : 500, out);
}

/**
 * @brief Handle test email sending requests
 */
void handleEmailSend() {
  // Validate request body
  if (!server.hasArg("plain")) { 
    sendText(400, "Invalid JSON"); 
    return; 
  }
  
  // Parse JSON request
  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, server.arg("plain"))) { 
    sendText(400, "Invalid JSON"); 
    return; 
  }
  
  // Extract email parameters
  String toEmail = doc["to"] | "";
  String subject = doc["subject"] | "Test Email from ESP32 Dashboard";
  String content = doc["content"] | "This is a test email sent from your ESP32 IoT Configuration Panel.";
  
  // Validate required parameters
  if (!toEmail.length()) { 
    sendText(400, "Recipient email required"); 
    return; 
  }

  // Check if email configuration is valid
  if (!emailCfg.isValid()) {
    sendText(400, "Email configuration incomplete. Please configure SMTP settings first.");
    return;
  }
  
  // Log email attempt
  Serial.print("Sending test email to: ");
  Serial.println(toEmail);
  Serial.print("Subject: ");
  Serial.println(subject);
  
  // Send email via SMTP (WiFi)
  bool success = sendEmailWiFi(toEmail, subject, content);
  
  // Build response
  DynamicJsonDocument resp(256);
  resp["success"] = success;
  if (success) {
    resp["message"] = "Test email sent successfully";
  } else {
    resp["error"] = "Failed to send test email. Check SMTP configuration and network connection.";
  }
  
  String out; 
  serializeJson(resp, out);
  sendJson(success ? 200 : 500, out);
}

/**
 * @brief Handle simple dummy email sending (legacy endpoint)
 */
void handleDummyEmail() {
  String toEmail = server.arg("to");
  String response;

  if (toEmail.isEmpty()) {
    server.send(400, "application/json", "{\"message\":\"Email is empty!\"}");
    return;
  }

  // Check if email configuration is valid
  if (!emailCfg.isValid()) {
    response = "{\"message\":\"Email configuration incomplete!\"}";
    server.send(400, "application/json", response);
    return;
  }

  // Send test email via WiFi
  bool success = sendEmailWiFi(toEmail, 
                              "Dummy Test Email from ESP32", 
                              "This is a dummy test email sent from your ESP32 IoT Configuration Panel.");

  if (success) {
    response = "{\"message\":\"Email sent successfully!\"}";
  } else {
    response = "{\"message\":\"Email sending failed!\"}";
  }

  server.send(200, "application/json", response);
}

// ============================================================================
// DRD (DEVICE RESET DETECTION) FUNCTIONS
// ============================================================================

// Forward declaration
void processButtonPress(unsigned long currentTime);

/**
 * @brief DRD button interrupt handler with enhanced debouncing
 * 
 * @details
 * This interrupt handler implements a state machine for robust button detection.
 * It handles debouncing, press/release detection, and long press detection.
 * The interrupt is triggered on both rising and falling edges for complete detection.
 */
void IRAM_ATTR drdButtonISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  // Basic hardware debounce: ignore interrupts that occur too quickly
  if (interruptTime - lastInterruptTime > 10) { // 10ms hardware debounce
    bool currentState = digitalRead(DRD_BUTTON_PIN);
    
    // Update state change time
    drdStateChangeTime = interruptTime;
    
    if (currentState == LOW) { // Button pressed (assuming pull-up resistor)
      if (drdButtonState == DRD_IDLE) {
        drdButtonState = DRD_PRESSED;
        drdPressStartTime = interruptTime;
      }
    } else { // Button released
      if (drdButtonState == DRD_PRESSED || drdButtonState == DRD_HELD) {
        drdButtonState = DRD_RELEASED;
      }
    }
    
    lastInterruptTime = interruptTime;
  }
}

/**
 * @brief Enhanced DRD state machine with soft debouncing
 * 
 * @details
 * This function implements a complete state machine for button detection with:
 * - Soft debouncing using state transitions
 * - Press and release detection
 * - Long press detection (2+ seconds)
 * - Double-press detection within 5-second window
 * - Automatic state resets and timeouts
 * 
 * Should be called regularly from the main loop (every 10-50ms recommended).
 */
void checkDRD() {
  static unsigned long lastCheckTime = 0;
  unsigned long currentTime = millis();
  
  // Check every 20ms for responsive detection
  if (currentTime - lastCheckTime < 20) {
    return;
  }
  lastCheckTime = currentTime;
  
  // State machine implementation
  switch (drdButtonState) {
    
    case DRD_IDLE:
      // Button is not pressed, waiting for press
      break;
      
    case DRD_PRESSED:
      // Button pressed, check if debounce time has passed
      if (currentTime - drdStateChangeTime >= DRD_DEBOUNCE_TIME) {
        // Verify button is still pressed (soft debounce)
        if (digitalRead(DRD_BUTTON_PIN) == LOW) {
          // Confirmed press - check for long press
          if (currentTime - drdPressStartTime >= DRD_HOLD_TIME) {
            drdButtonState = DRD_HELD;
            drdLongPressDetected = true;
            Serial.println("DRD Long press detected (" + String(DRD_HOLD_TIME/1000) + "s)");
          }
          // Stay in PRESSED state for short presses
        } else {
          // Button released during debounce - ignore this press
          drdButtonState = DRD_IDLE;
          Serial.println("DRD Press ignored (debounce)");
        }
      }
      break;
      
    case DRD_HELD:
      // Button held for long press, wait for release
      if (digitalRead(DRD_BUTTON_PIN) == HIGH) {
        // Button released, start release debounce
        drdButtonState = DRD_RELEASED;
        drdStateChangeTime = currentTime;
        Serial.println("DRD Long press released");
      }
      break;
      
    case DRD_RELEASED:
      // Button released, check if debounce time has passed
      if (currentTime - drdStateChangeTime >= DRD_DEBOUNCE_TIME) {
        // Verify button is still released (soft debounce)
        if (digitalRead(DRD_BUTTON_PIN) == HIGH) {
          // Confirmed release - process the press
          processButtonPress(currentTime);
        } else {
          // Button pressed again during debounce - ignore release
          drdButtonState = DRD_IDLE;
          Serial.println("DRD Release ignored (debounce)");
        }
      }
      break;
  }
  
  // Handle press count timeout
  if (drdPressCount > 0 && (currentTime - drdLastPressTime) > DRD_DOUBLE_PRESS_TIME) {
    Serial.println("DRD Timeout - resetting press count (" + String(drdPressCount) + " presses)");
    drdPressCount = 0;
  }
}

/**
 * @brief Process a confirmed button press
 * 
 * @details
 * This function handles the logic for processing a confirmed button press.
 * It manages press counting, double-press detection, and mode switching.
 * 
 * @param currentTime Current timestamp for timing calculations
 */
void processButtonPress(unsigned long currentTime) {
  // Reset to idle state
  drdButtonState = DRD_IDLE;
  
  // Increment press count
  drdPressCount++;
  drdLastPressTime = currentTime;
  
  Serial.println("DRD Button press confirmed - Count: " + String(drdPressCount));
  
  // Check for double press after a short delay
  static unsigned long lastProcessTime = 0;
  if (currentTime - lastProcessTime > 500) { // 500ms delay to allow for second press
    
    if (drdPressCount >= 2) {
      // Double press detected - switch modes
      drdConfigMode = !drdConfigMode;
      
      Serial.println("DRD Double-press detected! Switching to " + 
                    String(drdConfigMode ? "Configuration" : "Dashboard") + " mode");
      
      // Reset press count
      drdPressCount = 0;
      
      // Optional: Add visual feedback (LED blink, etc.)
      // digitalWrite(LED_PIN, HIGH);
      // delay(200);
      // digitalWrite(LED_PIN, LOW);
      
    } else if (drdPressCount == 1) {
      // Single press - check if it was a long press
      if (drdLongPressDetected) {
        Serial.println("DRD Long press detected - could be used for factory reset");
        // Future: Implement factory reset functionality
        drdLongPressDetected = false;
      } else {
        Serial.println("DRD Single press detected - resetting count");
      }
      drdPressCount = 0;
    }
    
    lastProcessTime = currentTime;
  }
}

/**
 * @brief Initialize DRD button and interrupt with enhanced features
 * 
 * @details
 * Sets up the DRD button pin with pull-up resistor and configures the interrupt
 * to trigger on both rising and falling edges for complete press detection.
 * Initializes the state machine and all timing variables.
 */
void initDRD() {
  // Configure button pin with internal pull-up resistor
  pinMode(DRD_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize state machine variables
  drdButtonState = DRD_IDLE;
  drdStateChangeTime = 0;
  drdPressStartTime = 0;
  drdLastPressTime = 0;
  drdPressCount = 0;
  drdConfigMode = false;
  drdLongPressDetected = false;
  
  // Attach interrupt for both rising and falling edges
  attachInterrupt(digitalPinToInterrupt(DRD_BUTTON_PIN), drdButtonISR, CHANGE);
  
  Serial.println("DRD Enhanced System initialized on GPIO" + String(DRD_BUTTON_PIN));
  Serial.println("Features: Soft debouncing, Long press detection, Double-press switching");
  Serial.println("Timing: " + String(DRD_DEBOUNCE_TIME) + "ms debounce, " + 
                String(DRD_HOLD_TIME/1000) + "s long press, " + 
                String(DRD_DOUBLE_PRESS_TIME/1000) + "s double-press window");
}

// ============================================================================
// MAIN APPLICATION SETUP
// ============================================================================

/**
 * @brief Main setup function - initializes all system components
 * 
 * @details
 * This function performs the complete system initialization:
 * 1. Serial communication setup for debugging
 * 2. SPIFFS filesystem mounting for configuration storage
 * 3. Configuration loading from saved files
 * 4. WiFi Access Point startup with captive portal
 * 5. WiFi Station mode connection attempt
 * 6. GSM modem initialization
 * 7. Web server and API endpoint configuration
 * 8. DRD (Device Reset Detection) initialization
 * 
 * The function sets up both WiFi modes (AP+STA) and initializes the GSM modem
 * for dual-mode operation. All configuration is loaded from SPIFFS files.
 */
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200); 
  delay(200);
  Serial.println("Starting ESP32 Configuration Panel...");

  // Mount SPIFFS filesystem for configuration storage
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    Serial.println("SPIFFS mounted successfully");
  }

  // Load all configuration from saved files
  wifiCfg.load(); 
  gsmCfg.load(); 
  userCfg.load();
  emailCfg.load();  // Load email configuration

  // Initialize DRD (Device Reset Detection) system
  initDRD();

  // Start Access Point with saved or default configuration
  String apSsid = wifiCfg.apSsid.length() ? wifiCfg.apSsid : DEFAULT_AP_SSID;
  String apPass = wifiCfg.apPass.length() ? wifiCfg.apPass : DEFAULT_AP_PASS;
  startAP(apSsid, apPass);

  // Log Access Point information
  Serial.println("Access Point started:");
  Serial.print("SSID: ");
  Serial.println(apSsid);
  Serial.print("IP: ");
  Serial.println(ipToStr(WiFi.softAPIP()));

  // Attempt to connect to saved WiFi network in Station mode
  if (wifiCfg.staSsid.length()) {
    Serial.print("Attempting to connect to saved WiFi: ");
    Serial.println(wifiCfg.staSsid);
    connectSTA(wifiCfg.staSsid, wifiCfg.staPass);
  }

  // Initialize GSM modem for cellular operations
  Serial.println("Initializing GSM modem...");
  gsmModem.begin();
  delay(2000);  // Give modem time to initialize and stabilize
  Serial.println("GSM modem initialized");

  // ============================================================================
  // WEB SERVER ROUTE CONFIGURATION
  // ============================================================================
  
  // Main dashboard routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/index.html", HTTP_GET, handleRoot);
  
  // Captive portal detection endpoints for various devices
  server.on("/generate_204", HTTP_GET, []() {
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
  });
  
  server.on("/ncsi.txt", HTTP_GET, []() {
    addCORS();
    server.send(200, "text/plain", "Microsoft NCSI");
  });
  
  // Additional captive portal endpoints for different platforms
  server.on("/hotspot-detect.html", HTTP_GET, []() {
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
  });
  
  server.on("/success.txt", HTTP_GET, []() {
    addCORS();
    server.send(200, "text/plain", "success");
  });
  
  // Test endpoint to verify server functionality
  server.on("/test", HTTP_GET, []() {
    addCORS();
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Server is working\"}");
  });

  // Error handling and CORS preflight routes
  server.onNotFound(handleNotFound);
  server.on("/", HTTP_OPTIONS, handleOptions);
  server.on("/index.html", HTTP_OPTIONS, handleOptions);

  // ============================================================================
  // API ENDPOINT CONFIGURATION
  // ============================================================================
  
  // System status API endpoint
  server.on("/api/status", HTTP_GET, []() { 
    sendJson(200, buildStatusJson()); 
  });
  server.on("/api/status", HTTP_OPTIONS, handleOptions);

  // WiFi network scanning API endpoint
  server.on("/api/wifi/scan", HTTP_GET, []() {
    Serial.println("Starting WiFi scan...");
    int n = WiFi.scanNetworks(false, true);
    Serial.print("Found ");
    Serial.print(n);
    Serial.println(" networks");

    // Create JSON document for network list
    DynamicJsonDocument doc(8192);
    
    // Structure to hold network information
    struct Row { 
      String ssid; 
      int rssi; 
      int channel; 
      wifi_auth_mode_t auth; 
    };
    
    // Collect and deduplicate networks (keep strongest signal for each SSID)
    std::vector<Row> rows; 
    rows.reserve(n);
    
    for (int i = 0; i < n; i++) {
      Row r{ WiFi.SSID(i), WiFi.RSSI(i), WiFi.channel(i), WiFi.encryptionType(i) };
      if (r.ssid.length() == 0) continue; // Skip empty SSIDs
      
      // Check if this SSID already exists and replace if signal is stronger
      bool replaced = false;
      for (auto &e : rows) { 
        if (e.ssid == r.ssid) { 
          if (r.rssi > e.rssi) e = r; 
          replaced = true; 
          break; 
        } 
      }
      if (!replaced) rows.push_back(r);
    }
    
    // Sort networks by signal strength (strongest first)
    std::sort(rows.begin(), rows.end(), [](const Row&a, const Row&b){ 
      return a.rssi > b.rssi; 
    });

    // Build JSON response
    JsonArray arr = doc.to<JsonArray>();
    for (auto &r : rows) {
      JsonObject o = arr.add<JsonObject>();
      o["ssid"]     = r.ssid;
      o["rssi"]     = r.rssi;
      o["channel"]  = r.channel;
      o["auth"]     = (int)r.auth;
      o["security"] = (r.auth == WIFI_AUTH_OPEN) ? "Open" : "Secure";
      o["strength"] = rssiToStrength(r.rssi);
    }
    
    String out; 
    serializeJson(doc, out);
    sendJson(200, out);
  });
  server.on("/api/wifi/scan", HTTP_OPTIONS, handleOptions);

  // WiFi connection API endpoint
  server.on("/api/wifi/connect", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(1024);
    auto err = deserializeJson(doc, server.arg("plain"));
    if (err) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract connection parameters
    String ssid = doc["ssid"] | "";
    String pass = doc["password"] | "";
    if (!ssid.length()) { 
      sendText(400, "SSID required"); 
      return; 
    }

    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    
    // Save configuration to SPIFFS for persistence
    wifiCfg.staSsid = ssid; 
    wifiCfg.staPass = pass; 
    wifiCfg.save();

    // Disconnect from current network and wait for cleanup
    WiFi.disconnect(true); 
    delay(1000);
    
    // Start connection process
    WiFi.begin(ssid.c_str(), pass.length() > 0 ? pass.c_str() : NULL);
    
    // Non-blocking connection check with progress updates
    unsigned long start = millis();
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) { 
      delay(200); 
      attempts++;
      
      // Keep server responsive during connection attempt
      if (attempts % 10 == 0) {
        Serial.print(".");
        server.handleClient(); // Process web requests
        dnsServer.processNextRequest(); // Handle captive portal
      }
    }
    Serial.println();

    // Build response based on connection result
    DynamicJsonDocument resp(512);
    bool ok = (WiFi.status() == WL_CONNECTED);
    resp["success"] = ok;
    
    if (ok) { 
      // Connection successful - return connection details
      resp["ip"] = ipToStr(WiFi.localIP()); 
      resp["ssid"] = WiFi.SSID(); 
      resp["rssi"] = WiFi.RSSI();
      resp["message"] = "Connected successfully";
      Serial.print("WiFi connected successfully to: ");
      Serial.println(WiFi.SSID());
    } else { 
      // Connection failed - return error information
      resp["ip"] = "0.0.0.0"; 
      resp["ssid"] = ""; 
      resp["error"] = "Connection failed - check password and signal strength";
      Serial.println("WiFi connection failed");
    }

    String out; 
    serializeJson(resp, out);
    sendJson(ok ? 200 : 500, out);
  });
  server.on("/api/wifi/connect", HTTP_OPTIONS, handleOptions);

  // WiFi disconnection API endpoint
  server.on("/api/wifi/disconnect", HTTP_POST, []() {
    Serial.println("Disconnecting from WiFi...");
    
    // Clear saved WiFi configuration
    wifiCfg.staSsid = ""; 
    wifiCfg.staPass = ""; 
    wifiCfg.save();
    
    // Disconnect from current network
    WiFi.disconnect(true); 
    delay(1000);
    
    // Ensure dual mode is maintained
    WiFi.mode(WIFI_AP_STA);
    
    // Return success response
    DynamicJsonDocument resp(256); 
    resp["success"] = true; 
    resp["message"] = "Disconnected from WiFi network";
    String out; 
    serializeJson(resp, out);
    sendJson(200, out);
  });
  server.on("/api/wifi/disconnect", HTTP_OPTIONS, handleOptions);

  // GSM configuration save API endpoint
  server.on("/api/save/gsm", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Update GSM configuration from request
    gsmCfg.carrierName = doc["carrierName"] | "";
    gsmCfg.apn         = doc["apn"] | "";
    gsmCfg.apnUser     = doc["apnUser"] | "";
    gsmCfg.apnPass     = doc["apnPass"] | "";
    
    // Save configuration to SPIFFS
    bool ok = gsmCfg.save();
    sendText(ok ? 200 : 500, ok ? "OK" : "SAVE_FAILED");
  });
  server.on("/api/save/gsm", HTTP_OPTIONS, handleOptions);

  // GSM configuration load API endpoint
  server.on("/api/load/gsm", HTTP_GET, []() {
    // Build JSON response with current GSM configuration
    DynamicJsonDocument doc(1024);
    doc["carrierName"] = gsmCfg.carrierName;
    doc["apn"]         = gsmCfg.apn;
    doc["apnUser"]     = gsmCfg.apnUser;
    doc["apnPass"]     = gsmCfg.apnPass;
    
    String out; 
    serializeJson(doc, out);
    sendJson(200, out);
  });
  server.on("/api/load/gsm", HTTP_OPTIONS, handleOptions);

  // GSM APN configuration API endpoint
  server.on("/api/gsm/setapn", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract APN configuration parameters
    String apn = doc["apn"] | "";
    String pdp = doc["pdp"] | "IP";
    String cid = String((int)(doc["cid"] | 1));
    
    if (!apn.length()) { 
      sendText(400, "APN required"); 
      return; 
    }

    // Log APN configuration details
    Serial.print("APN configuration: cid=");
    Serial.print(cid);
    Serial.print(" pdp=");
    Serial.print(pdp);
    Serial.print(" apn=");
    Serial.println(apn);
    
    bool ok = true; // APN configuration saved to file
    Serial.println("APN configuration saved to file");

    // Persist APN configuration to SPIFFS
    gsmCfg.apn = apn; 
    gsmCfg.save();

    // Return success response
    DynamicJsonDocument resp(128); 
    resp["success"] = ok;
    String out; 
    serializeJson(resp, out);
    sendJson(ok ? 200 : 500, out);
  });
  server.on("/api/gsm/setapn", HTTP_OPTIONS, handleOptions);

  // GSM signal strength API endpoint with caching
  server.on("/api/gsm/signal", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    
    // Check for force refresh parameter
    bool forceRefresh = server.hasArg("force") && server.arg("force") == "true";
    
    // Update cache only if needed (5-minute intervals or force refresh)
    if (gsmCache.needsUpdate(forceRefresh)) {
      if (forceRefresh) {
        Serial.println("Force refreshing GSM signal data");
      } else {
        Serial.println("Updating GSM signal data (5-minute interval)");
      }
      gsmCache.updateSignal(forceRefresh);
    } else {
      Serial.println("Using cached GSM signal data");
    }
    
    // Build response with signal information
    doc["ok"] = (gsmCache.signalStrength != -999);
    if (forceRefresh) {
      doc["raw"] = "Signal strength force refreshed";
    } else if (gsmCache.needsUpdate()) {
      doc["raw"] = "Signal strength updated";
    } else {
      doc["raw"] = "Using cached data";
    }
    doc["dbm"] = gsmCache.signalStrength;
    doc["csq"] = gsmCache.signalQuality;
    doc["ber"] = 99; // BER not available from getSignalStrength()
    doc["grade"] = gsmCache.grade;
    doc["lastUpdate"] = gsmCache.lastUpdate;
    doc["nextUpdate"] = gsmCache.lastUpdate + gsmCache.UPDATE_INTERVAL;
    doc["forceRefresh"] = forceRefresh;
    
    String out; 
    serializeJson(doc, out);
    sendJson(200, out);
  });
  server.on("/api/gsm/signal", HTTP_OPTIONS, handleOptions);

  // GSM network information API endpoint with caching
  server.on("/api/gsm/network", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    
    // Check for force refresh parameter
    bool forceRefresh = server.hasArg("force") && server.arg("force") == "true";
    
    // Update cache only if needed (5-minute intervals or force refresh)
    if (gsmCache.needsUpdate(forceRefresh)) {
      if (forceRefresh) {
        Serial.println("Force refreshing GSM network data");
      } else {
        Serial.println("Updating GSM network data (5-minute interval)");
      }
      gsmCache.updateNetwork(forceRefresh);
    } else {
      Serial.println("Using cached GSM network data");
    }
    
    // Build response with network information
    doc["carrierName"] = gsmCache.carrierName;
    doc["networkMode"] = gsmCache.networkMode;
    doc["isRegistered"] = gsmCache.isRegistered;
    doc["signalStrength"] = gsmCache.signalStrength;
    doc["signalQuality"] = gsmCache.signalQuality;
    doc["lastUpdate"] = gsmCache.lastUpdate;
    doc["nextUpdate"] = gsmCache.lastUpdate + gsmCache.UPDATE_INTERVAL;
    doc["forceRefresh"] = forceRefresh;
    
    String out; 
    serializeJson(doc, out);
    sendJson(200, out);
  });
  server.on("/api/gsm/network", HTTP_OPTIONS, handleOptions);

  // GSM SMS sending API endpoint
  server.on("/api/gsm/sms", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract SMS parameters
    String phoneNumber = doc["phoneNumber"] | "";
    String message = doc["message"] | "";
    
    // Validate required parameters
    if (!phoneNumber.length()) { 
      sendText(400, "Phone number required"); 
      return; 
    }
    if (!message.length()) { 
      sendText(400, "Message required"); 
      return; 
    }
    
    // Log SMS attempt
    Serial.print("Sending SMS to: ");
    Serial.println(phoneNumber);
    Serial.print("Message: ");
    Serial.println(message);
    
    // Send SMS via GSM modem
    bool success = gsmModem.sendSMS(phoneNumber, message);
    
    // Build response
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    if (success) {
      resp["message"] = "SMS sent successfully";
    } else {
      resp["error"] = "Failed to send SMS";
    }
    
    String out; 
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  server.on("/api/gsm/sms", HTTP_OPTIONS, handleOptions);

  // GSM voice call API endpoint
  server.on("/api/gsm/call", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract phone number
    String phoneNumber = doc["phoneNumber"] | "";
    
    // Validate required parameters
    if (!phoneNumber.length()) { 
      sendText(400, "Phone number required"); 
      return; 
    }
    
    // Log call attempt
    Serial.print("Making call to: ");
    Serial.println(phoneNumber);
    
    // Initiate call via GSM modem
    bool success = gsmModem.makeCall(phoneNumber);
    
    // Build response
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    if (success) {
      resp["message"] = "Call initiated successfully";
      
      // Schedule automatic hangup after 10 seconds for test calls
      delay(10000);
      gsmModem.hangupCall();
      Serial.println("Test call ended after 10 seconds");
    } else {
      resp["error"] = "Failed to initiate call";
    }
    
    String out; 
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  server.on("/api/gsm/call", HTTP_OPTIONS, handleOptions);

  // ============================================================================
  // EMAIL API ENDPOINTS
  // ============================================================================

  // Email configuration load API endpoint
  server.on("/api/load/email", HTTP_GET, handleEmailLoad);
  server.on("/api/load/email", HTTP_OPTIONS, handleOptions);

  // Email configuration save API endpoint
  server.on("/api/save/email", HTTP_POST, handleEmailSave);
  server.on("/api/save/email", HTTP_OPTIONS, handleOptions);

  // Email test send API endpoint
  server.on("/api/email/send", HTTP_POST, handleEmailSend);
  server.on("/api/email/send", HTTP_OPTIONS, handleOptions);

  // Legacy dummy email endpoint (for compatibility)
  server.on("/sendDummyEmail", HTTP_GET, handleDummyEmail);
  server.on("/sendDummyEmail", HTTP_OPTIONS, handleOptions);

  // GSM Email API endpoints
  server.on("/api/email/gsm/send", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract email parameters
    String toEmail = doc["to"] | "";
    String subject = doc["subject"] | "Test Email via GSM from ESP32 Dashboard";
    String content = doc["content"] | "This is a test email sent via GSM from your ESP32 IoT Configuration Panel.";
    
    // Validate required parameters
    if (!toEmail.length()) { 
      sendText(400, "Recipient email required"); 
      return; 
    }

    // Check if email configuration is valid
    if (!emailCfg.isValid()) {
      sendText(400, "Email configuration incomplete. Please configure SMTP settings first.");
      return;
    }
    
    // Log GSM email attempt
    Serial.print("Sending GSM email to: ");
    Serial.println(toEmail);
    Serial.print("Subject: ");
    Serial.println(subject);
    
    // Send email via GSM SMTP
    bool success = sendEmailGSM(toEmail, subject, content);
    
    // Build response
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    if (success) {
      resp["message"] = "GSM email sent successfully";
      resp["method"] = "GSM";
    } else {
      resp["error"] = "Failed to send email via GSM. Check GSM network connection and SMTP configuration.";
      resp["method"] = "GSM";
    }
    
    String out; 
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  server.on("/api/email/gsm/send", HTTP_OPTIONS, handleOptions);

  // User profile load API endpoint
  server.on("/api/load/user", HTTP_GET, []() {
    // Build JSON response with current user configuration
    DynamicJsonDocument doc(1024);
    doc["name"] = userCfg.name;
    doc["email"] = userCfg.email;
    doc["phone"] = userCfg.phone;
    
    String out; 
    serializeJson(doc, out);
    sendJson(200, out);
  });
  server.on("/api/load/user", HTTP_OPTIONS, handleOptions);

  // User profile save API endpoint
  server.on("/api/save/user", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Update user configuration from request
    userCfg.name  = doc["name"]  | "";
    userCfg.email = doc["email"] | "";
    userCfg.phone = doc["phone"] | "";
    
    // Save configuration to SPIFFS
    bool ok = userCfg.save();

    // Log user data save operation
    Serial.print("User data saved: ");
    Serial.print(userCfg.name);
    Serial.print(", ");
    Serial.print(userCfg.email);
    Serial.print(", ");
    Serial.println(userCfg.phone);

    // Return JSON response with success status
    DynamicJsonDocument resp(256);
    resp["success"] = ok;
    if (ok) {
      resp["message"] = "User data saved successfully";
    } else {
      resp["message"] = "Failed to save user data";
    }
    
    String out; 
    serializeJson(resp, out);
    sendJson(ok ? 200 : 500, out);
  });
  server.on("/api/save/user", HTTP_OPTIONS, handleOptions);

  // WiFi Access Point settings save API endpoint
  server.on("/api/save/wifi", HTTP_POST, []() {
    // Validate request body
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Parse JSON request
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    // Extract AP configuration parameters
    String apSsid = doc["apSsid"] | wifiCfg.apSsid;
    String apPass = doc["apPass"] | wifiCfg.apPass;
    
    // Update configuration if valid values provided
    if (apSsid.length()) wifiCfg.apSsid = apSsid;
    if (apPass.length()) wifiCfg.apPass = apPass;
    
    // Save configuration to SPIFFS
    bool ok = wifiCfg.save();
    sendText(ok ? 200 : 500, ok ? "OK" : "SAVE_FAILED");
  });
  server.on("/api/save/wifi", HTTP_OPTIONS, handleOptions);

  // System reboot API endpoint
  server.on("/api/reboot", HTTP_POST, []() {
    sendText(200, "OK"); 
    delay(200); 
    ESP.restart();
  });
  server.on("/api/reboot", HTTP_OPTIONS, handleOptions);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("Configuration panel available at: http://");
  Serial.println(ipToStr(WiFi.softAPIP()));
}

// ============================================================================
// MAIN APPLICATION LOOP
// ============================================================================

/**
 * @brief Main application loop - handles ongoing operations
 * 
 * @details
 * This function runs continuously and handles:
 * 1. DNS server requests for captive portal functionality
 * 2. Web server client requests and API calls
 * 3. Periodic status logging every 30 seconds
 * 
 * The loop is designed to be non-blocking and responsive to web requests
 * while maintaining system status monitoring.
 */
void loop() {
  // Process captive portal DNS requests
  dnsServer.processNextRequest();
  
  // Handle web server client requests
  server.handleClient();

  // Check for DRD (Device Reset Detection) button presses
  checkDRD();

  // Periodic status logging every 30 seconds
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 30000) {
    lastStatusPrint = millis();
    Serial.print("Status - AP: ");
    Serial.print(ipToStr(WiFi.softAPIP()));
    Serial.print(", STA: ");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print(ipToStr(WiFi.localIP()));
    } else {
      Serial.print("Not connected");
    }
    Serial.print(", Email: ");
    Serial.println(emailCfg.isValid() ? "Configured" : "Not configured");
  }
}