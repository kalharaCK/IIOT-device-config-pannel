/**
 * ESP32 Configuration Panel with Double Reset Detection
 * 
 * Features:
 * - Single reset: Main dashboard (WiFi + GSM management)
 * - Double reset: Email configuration dashboard
 * - Automatic dashboard switching via DRD
 * 
 * Double Reset: Press reset button twice within 3 seconds
 * 
 * @version 2.3.0
 * @date 2025-01-30
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "GSM_Test.h"
#include "SMTP.h"
#include "DRD_Manager.h"
#include "dashboard_html.h"  // Main dashboard
#include "config_html.h"     // Email config dashboard

// ============================================================================
// GLOBAL CONFIGURATION
// ============================================================================
#define DNS_PORT 53
#define DRD_TIMEOUT 3000  // 3 seconds for double reset detection

// ============================================================================
// DASHBOARD MODE
// ============================================================================
/**
 * @brief Dashboard operating modes
 * MODE_MAIN: Main dashboard with WiFi and GSM management
 * MODE_EMAIL: Email configuration dashboard
 */
enum DashboardMode {
  MODE_MAIN,     // Main dashboard (WiFi + GSM)
  MODE_EMAIL     // Email configuration dashboard
};

DashboardMode currentMode = MODE_MAIN;

// ============================================================================
// INSTANCES
// ============================================================================
DNSServer dnsServer;              // DNS server for captive portal
WebServer server(80);             // HTTP web server on port 80
DRD_Manager drd(DRD_TIMEOUT);     // Double reset detector

// GSM instances
GSM_Test gsmModem(Serial2, 16, 17, 115200);  // GSM modem on Serial2 (RX=16, TX=17)
SMTP smtp(Serial2, 16, 17, 115200);          // SMTP client for GSM email

// ============================================================================
// CONFIGURATION FILES
// ============================================================================
static const char* WIFI_FILE = "/wifi.json";
static const char* GSM_FILE = "/gsm.json";
static const char* USER_FILE = "/user.json";
static const char* EMAIL_FILE = "/email.json";
static const char* DEFAULT_AP_SSID = "ESP32-AccessPoint";
static const char* DEFAULT_AP_PASS = "12345678";

// ============================================================================
// CONFIGURATION STRUCTURES
// ============================================================================

/**
 * @brief WiFi Configuration Structure
 * Stores both Access Point and Station mode settings
 */
struct WifiConfig {
  String staSsid;  // Station mode SSID (client mode)
  String staPass;  // Station mode password
  String apSsid;   // Access Point SSID
  String apPass;   // Access Point password

  /**
   * @brief Load WiFi configuration from SPIFFS
   * @return true if loaded successfully, false otherwise
   */
  bool load() {
    if (!SPIFFS.exists(WIFI_FILE)) return false;
    File f = SPIFFS.open(WIFI_FILE, "r");
    if (!f) return false;
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, f)) { f.close(); return false; }
    f.close();
    staSsid = doc["staSsid"] | "";
    staPass = doc["staPass"] | "";
    apSsid = doc["apSsid"] | DEFAULT_AP_SSID;
    apPass = doc["apPass"] | DEFAULT_AP_PASS;
    return true;
  }

  /**
   * @brief Save WiFi configuration to SPIFFS
   * @return true if saved successfully, false otherwise
   */
  bool save() const {
    DynamicJsonDocument doc(1024);
    doc["staSsid"] = staSsid;
    doc["staPass"] = staPass;
    doc["apSsid"] = apSsid.length() ? apSsid : DEFAULT_AP_SSID;
    doc["apPass"] = apPass.length() ? apPass : DEFAULT_AP_PASS;
    File f = SPIFFS.open(WIFI_FILE, "w");
    if (!f) return false;
    serializeJson(doc, f);
    f.close();
    return true;
  }
} wifiCfg;

/**
 * @brief GSM Configuration Structure
 * Stores carrier and APN settings for GSM connectivity
 */
struct GsmConfig {
  String carrierName;  // Network carrier name (e.g., "Dialog", "Mobitel")
  String apn;          // Access Point Name for data connection
  String apnUser;      // APN username (if required)
  String apnPass;      // APN password (if required)

  /**
   * @brief Load GSM configuration from SPIFFS
   * @return true if loaded successfully, false otherwise
   */
  bool load() {
    if (!SPIFFS.exists(GSM_FILE)) return false;
    File f = SPIFFS.open(GSM_FILE, "r");
    if (!f) return false;
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, f)) { f.close(); return false; }
    f.close();
    carrierName = doc["carrierName"] | "";
    apn = doc["apn"] | "";
    apnUser = doc["apnUser"] | "";
    apnPass = doc["apnPass"] | "";
    return true;
  }

  /**
   * @brief Save GSM configuration to SPIFFS
   * @return true if saved successfully, false otherwise
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
 * @brief User Profile Configuration Structure
 * Stores user information for the dashboard
 */
struct UserConfig {
  String name;   // User's full name
  String email;  // User's email address
  String phone;  // User's phone number

  /**
   * @brief Load user configuration from SPIFFS
   * @return true if loaded successfully, false otherwise
   */
  bool load() {
    if (!SPIFFS.exists(USER_FILE)) return false;
    File f = SPIFFS.open(USER_FILE, "r");
    if (!f) return false;
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, f)) { f.close(); return false; }
    f.close();
    name = doc["name"] | "";
    email = doc["email"] | "";
    phone = doc["phone"] | "";
    return true;
  }

  /**
   * @brief Save user configuration to SPIFFS
   * @return true if saved successfully, false otherwise
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
 * @brief Email Configuration Structure
 * Stores SMTP settings for email functionality
 */
struct EmailConfig {
  String smtpHost = "smtp.gmail.com";  // SMTP server hostname
  int smtpPort = 465;                   // SMTP server port (465 for SSL)
  String emailAccount = "";             // Email account for authentication
  String emailPassword = "";            // App password for authentication
  String senderName = "ESP32 Device";   // Display name for sender

  /**
   * @brief Load email configuration from SPIFFS
   * @return true if loaded successfully, false otherwise
   */
  bool load() {
    if (!SPIFFS.exists(EMAIL_FILE)) return false;
    File f = SPIFFS.open(EMAIL_FILE, "r");
    if (!f) return false;
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, f)) { f.close(); return false; }
    f.close();
    smtpHost = doc["smtpHost"] | smtpHost;
    smtpPort = doc["smtpPort"] | smtpPort;
    emailAccount = doc["emailAccount"] | emailAccount;
    emailPassword = doc["emailPassword"] | emailPassword;
    senderName = doc["senderName"] | senderName;
    return true;
  }

  /**
   * @brief Save email configuration to SPIFFS
   * @return true if saved successfully, false otherwise
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
   * @return true if all required fields are filled, false otherwise
   */
  bool isValid() const {
    return smtpHost.length() > 0 && emailAccount.length() > 0 && emailPassword.length() > 0;
  }
} emailCfg;

// ============================================================================
// GSM CACHE
// ============================================================================
/**
 * @brief GSM Status Cache Structure
 * Caches GSM signal and network information to reduce modem queries
 * Updates every 5 minutes or on force refresh
 */
struct GSMCache {
  int signalStrength = 0;               // Signal strength in dBm
  int signalQuality = 99;               // Signal quality (0-31 scale)
  String grade = "Unknown";             // Signal grade (Excellent/Good/Fair/Poor)
  String carrierName = "Unknown";       // Network carrier name
  String networkMode = "Unknown";       // Network mode (GSM/LTE/etc)
  bool isRegistered = false;            // Network registration status
  unsigned long lastUpdate = 0;         // Timestamp of last update
  const unsigned long UPDATE_INTERVAL = 300000; // 5 minutes cache duration

  /**
   * @brief Check if cache needs update
   * @param forceRefresh Force immediate update regardless of cache age
   * @return true if update needed, false if cache is still valid
   */
  bool needsUpdate(bool forceRefresh = false) {
    return forceRefresh || (millis() - lastUpdate) > UPDATE_INTERVAL;
  }

  /**
   * @brief Update signal strength cache
   * @param forceRefresh Force immediate update
   */
  void updateSignal(bool forceRefresh = false) {
    if (needsUpdate(forceRefresh)) {
      signalStrength = gsmModem.getSignalStrength();
      if (signalStrength != 0) {
        // Convert dBm to CSQ scale (0-31)
        signalQuality = (signalStrength + 113) / 2;
        if (signalQuality < 0) signalQuality = 0;
        if (signalQuality > 31) signalQuality = 31;
        
        // Determine signal grade
        if (signalQuality >= 20) grade = "Excellent";
        else if (signalQuality >= 15) grade = "Good";
        else if (signalQuality >= 10) grade = "Fair";
        else grade = "Poor";
      }
      lastUpdate = millis();
    }
  }

  /**
   * @brief Update network information cache
   * @param forceRefresh Force immediate update
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
// UTILITIES
// ============================================================================

/**
 * @brief Convert IPAddress to String format
 * @param ip IPAddress object to convert
 * @return String representation of IP address (e.g., "192.168.4.1")
 */
String ipToStr(const IPAddress &ip) {
  char buf[24];
  snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  return String(buf);
}

/**
 * @brief Add CORS headers to HTTP response
 * Enables cross-origin requests from web browsers
 */
void addCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Cache-Control", "no-store");
}

/**
 * @brief Send JSON response with CORS headers
 * @param code HTTP status code
 * @param body JSON string body
 */
void sendJson(int code, const String& body) {
  addCORS();
  server.send(code, "application/json", body);
}

/**
 * @brief Send text response with CORS headers
 * @param code HTTP status code
 * @param body Response body text
 * @param ctype Content type (default: "text/plain")
 */
void sendText(int code, const String& body, const String& ctype = "text/plain") {
  addCORS();
  server.send(code, ctype, body);
}

// ============================================================================
// WIFI MANAGEMENT
// ============================================================================

/**
 * @brief Start WiFi Access Point
 * @param ssid AP SSID name
 * @param pass AP password (minimum 8 characters)
 */
void startAP(const String& ssid, const String& pass) {
  WiFi.mode(WIFI_AP_STA);  // Enable both AP and Station modes
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  bool ap_ok = WiFi.softAP(ssid.c_str(), pass.length() >= 8 ? pass.c_str() : NULL);
  if (!ap_ok) WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);  // Fallback to default
  delay(500);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());  // Start DNS server for captive portal
}

/**
 * @brief Connect to WiFi network in Station mode
 * @param ssid Network SSID to connect to
 * @param pass Network password
 */
void connectSTA(const String& ssid, const String& pass) {
  if (!ssid.length()) return;
  WiFi.begin(ssid.c_str(), pass.c_str());
}

/**
 * @brief Convert RSSI value to signal strength description
 * @param rssi Signal strength in dBm
 * @return "strong", "medium", or "weak"
 */
const char* rssiToStrength(int rssi) {
  if (rssi >= -60) return "strong";
  if (rssi >= -75) return "medium";
  return "weak";
}

// ============================================================================
// EMAIL SENDING
// ============================================================================

/**
 * @brief Send email via GSM network
 * @param toEmail Recipient email address
 * @param subject Email subject line
 * @param content Email body content
 * @return true if email sent successfully, false otherwise
 */
bool sendEmailGSM(const String& toEmail, const String& subject, const String& content) {
  Serial.println("📧 Sending email via GSM...");
  
  // Validate email configuration
  if (!emailCfg.isValid()) {
    Serial.println("⚠ Email configuration incomplete");
    return false;
  }

  // Initialize and configure SMTP client
  smtp.begin();
  smtp.setAPN(gsmCfg.apn.length() ? gsmCfg.apn.c_str() : "internet");
  smtp.setAuth(emailCfg.emailAccount.c_str(), emailCfg.emailPassword.c_str());
  smtp.setRecipient(toEmail.c_str());
  smtp.setFromName(emailCfg.senderName.c_str());
  smtp.setSubject(subject);
  smtp.setBody(content);

  return smtp.sendEmail();
}

// ============================================================================
// JSON BUILDERS
// ============================================================================

/**
 * @brief Build system status JSON response
 * @return JSON string containing complete system status
 */
String buildStatusJson() {
  DynamicJsonDocument doc(1024);
  doc["mode"] = "AP+STA";
  doc["dashboardMode"] = (currentMode == MODE_MAIN) ? "main" : "email";

  // Access Point information
  JsonObject ap = doc.createNestedObject("ap");
  ap["ssid"] = wifiCfg.apSsid.length() ? wifiCfg.apSsid : DEFAULT_AP_SSID;
  ap["ip"] = ipToStr(WiFi.softAPIP());
  ap["mac"] = WiFi.softAPmacAddress();
  ap["connectedDevices"] = WiFi.softAPgetStationNum();

  // Station mode information
  JsonObject sta = doc.createNestedObject("sta");
  bool staConnected = (WiFi.status() == WL_CONNECTED);
  sta["ssid"] = staConnected ? WiFi.SSID() : "";
  sta["connected"] = staConnected;
  sta["ip"] = staConnected ? ipToStr(WiFi.localIP()) : "0.0.0.0";
  sta["rssi"] = staConnected ? WiFi.RSSI() : 0;
  sta["hostname"] = WiFi.getHostname() ? WiFi.getHostname() : "";

  // Connection status message
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
// HTTP HANDLERS - COMMON
// ============================================================================

/**
 * @brief Handle root HTTP request
 * Serves appropriate dashboard based on current mode
 */
void handleRoot() {
  addCORS();
  if (currentMode == MODE_MAIN) {
    server.send_P(200, "text/html", dashboard_html, dashboard_html_len);
  } else {
    server.send_P(200, "text/html", config_html, config_html_len);
  }
}

/**
 * @brief Handle HTTP OPTIONS request for CORS preflight
 */
void handleOptions() {
  addCORS();
  server.send(204);
}

/**
 * @brief Handle 404 Not Found and captive portal detection
 */
void handleNotFound() {
  String host = server.hostHeader();
  
  // Check for captive portal detection requests
  if (host.startsWith("connectivitycheck.") || 
      host.startsWith("captive.apple.com") ||
      host.startsWith("msftconnecttest.") || 
      host.startsWith("detectportal.")) {
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
  } else {
    handleRoot();  // Serve dashboard for unknown paths
  }
}

// ============================================================================
// HTTP HANDLERS - MODE SWITCHING
// ============================================================================

/**
 * @brief Handle mode info request
 * Returns current mode and instructions for switching
 */
void handleSwitchMode() {
  DynamicJsonDocument doc(256);
  doc["currentMode"] = (currentMode == MODE_MAIN) ? "main" : "email";
  doc["message"] = "To switch modes, perform a double reset (reset twice within 3 seconds)";
  String out;
  serializeJson(doc, out);
  sendJson(200, out);
}

/**
 * @brief Handle mode switch request
 * Informs user that mode switching requires device reset
 */
void handleModeSwitchRequest() {
  if (!server.hasArg("plain")) {
    sendText(400, "Invalid JSON");
    return;
  }
  
  DynamicJsonDocument doc(256);
  if (deserializeJson(doc, server.arg("plain"))) {
    sendText(400, "Invalid JSON");
    return;
  }
  
  String mode = doc["mode"] | "";
  if (mode != "main" && mode != "email") {
    sendText(400, "Invalid mode. Use 'main' or 'email'");
    return;
  }
  
  // Mode switching requires physical reset, not runtime switch
  DynamicJsonDocument resp(256);
  resp["success"] = false;
  resp["message"] = "Mode switching requires device reset. Double-reset to switch.";
  resp["currentMode"] = (currentMode == MODE_MAIN) ? "main" : "email";
  String out;
  serializeJson(resp, out);
  sendJson(200, out);
}

// ============================================================================
// SETUP MAIN DASHBOARD ROUTES
// ============================================================================

/**
 * @brief Setup API endpoints for main dashboard
 * Includes WiFi, GSM, call, SMS, and configuration endpoints
 */
void setupMainDashboardRoutes() {
  Serial.println("⚙ Setting up MAIN dashboard routes");
  
  // ============================================================================
  // STATUS ENDPOINT
  // ============================================================================
  
  /**
   * GET /api/status
   * Returns complete system status including WiFi and GSM information
   */
  server.on("/api/status", HTTP_GET, []() { 
    sendJson(200, buildStatusJson()); 
  });
  
  // ============================================================================
  // GSM SIGNAL ENDPOINT
  // ============================================================================
  
  /**
   * GET /api/gsm/signal?force=true
   * Returns GSM signal strength and quality
   * Optional: force=true to bypass cache
   */
  server.on("/api/gsm/signal", HTTP_GET, []() {
    bool forceRefresh = server.hasArg("force") && server.arg("force") == "true";
    gsmCache.updateSignal(forceRefresh);
    
    DynamicJsonDocument doc(256);
    doc["ok"] = (gsmCache.signalStrength != -999);
    doc["dbm"] = gsmCache.signalStrength;
    doc["csq"] = gsmCache.signalQuality;
    doc["grade"] = gsmCache.grade;
    
    String out;
    serializeJson(doc, out);
    sendJson(200, out);
  });
  
  // ============================================================================
  // GSM NETWORK ENDPOINT
  // ============================================================================
  
  /**
   * GET /api/gsm/network?force=true
   * Returns GSM network information (carrier, mode, registration status)
   * Optional: force=true to bypass cache
   */
  server.on("/api/gsm/network", HTTP_GET, []() {
    bool forceRefresh = server.hasArg("force") && server.arg("force") == "true";
    gsmCache.updateNetwork(forceRefresh);
    
    DynamicJsonDocument doc(1024);
    doc["carrierName"] = gsmCache.carrierName;
    doc["networkMode"] = gsmCache.networkMode;
    doc["isRegistered"] = gsmCache.isRegistered;
    
    String out;
    serializeJson(doc, out);
    sendJson(200, out);
  });
  
  // ============================================================================
  // GSM CALL ENDPOINTS
  // ============================================================================
  
  /**
   * POST /api/gsm/call
   * Make a voice call via GSM modem
   * Request body: {"phoneNumber": "+94719792341"}
   * Call duration: 10 seconds (auto-hangup)
   */
  server.on("/api/gsm/call", HTTP_POST, []() {
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    DynamicJsonDocument doc(256);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    String phoneNumber = doc["phoneNumber"] | "";
    if (!phoneNumber.length()) { 
      DynamicJsonDocument resp(256);
      resp["success"] = false;
      resp["error"] = "Phone number required";
      String out;
      serializeJson(resp, out);
      sendJson(400, out);
      return; 
    }
    
    Serial.printf("📞 Making call to: %s\n", phoneNumber.c_str());
    
    // Make the call
    bool success = gsmModem.makeCall(phoneNumber);
    
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    
    if (success) {
      resp["message"] = "Call initiated successfully";
      
      // Keep call active for 10 seconds then hang up
      Serial.println("📞 Call active for 10 seconds...");
      delay(10000);
      
      Serial.println("📞 Hanging up call...");
      gsmModem.hangupCall();
      resp["message"] = "Call completed (10 seconds)";
    } else {
      resp["error"] = "Failed to initiate call";
    }
    
    String out;
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  
  /**
   * POST /api/gsm/call/hangup
   * Hang up active call
   */
  server.on("/api/gsm/call/hangup", HTTP_POST, []() {
    Serial.println("📞 Hanging up call...");
    bool success = gsmModem.hangupCall();
    
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    if (success) {
      resp["message"] = "Call ended successfully";
    } else {
      resp["error"] = "Failed to hang up call";
    }
    
    String out;
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  
  // ============================================================================
  // GSM SMS ENDPOINT
  // ============================================================================
  
  /**
   * POST /api/gsm/sms
   * Send SMS message via GSM modem
   * Request body: {"phoneNumber": "+94719792341", "message": "Test message"}
   */
  server.on("/api/gsm/sms", HTTP_POST, []() {
    if (!server.hasArg("plain")) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, server.arg("plain"))) { 
      sendText(400, "Invalid JSON"); 
      return; 
    }
    
    String phoneNumber = doc["phoneNumber"] | "";
    String message = doc["message"] | "";
    
    // Validate phone number
    if (!phoneNumber.length()) { 
      DynamicJsonDocument resp(256);
      resp["success"] = false;
      resp["error"] = "Phone number required";
      String out;
      serializeJson(resp, out);
      sendJson(400, out);
      return; 
    }
    
    // Validate message content
    if (!message.length()) { 
      DynamicJsonDocument resp(256);
      resp["success"] = false;
      resp["error"] = "Message content required";
      String out;
      serializeJson(resp, out);
      sendJson(400, out);
      return; 
    }
    
    Serial.printf("💬 Sending SMS to: %s\n", phoneNumber.c_str());
    Serial.printf("   Message: %s\n", message.c_str());
    
    // Send the SMS
    bool success = gsmModem.sendSMS(phoneNumber, message);
    
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
  
  // ============================================================================
  // USER CONFIGURATION ENDPOINTS
  // ============================================================================
  
  /**
   * GET /api/load/user
   * Load user profile configuration
   */
  server.on("/api/load/user", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    doc["name"] = userCfg.name;
    doc["email"] = userCfg.email;
    doc["phone"] = userCfg.phone;
    
    String out;
    serializeJson(doc, out);
    sendJson(200, out);
  });
  
  /**
   * POST /api/save/user
   * Save user profile configuration
   * Request body: {"name": "...", "email": "...", "phone": "..."}
   */
  server.on("/api/save/user", HTTP_POST, []() {
    if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
    
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
    
    userCfg.name = doc["name"] | "";
    userCfg.email = doc["email"] | "";
    userCfg.phone = doc["phone"] | "";
    
    bool ok = userCfg.save();
    
    DynamicJsonDocument resp(256);
    resp["success"] = ok;
    
    String out;
    serializeJson(resp, out);
    sendJson(ok ? 200 : 500, out);
  });
  
  // ============================================================================
  // GSM CONFIGURATION ENDPOINTS
  // ============================================================================
  
  /**
   * GET /api/load/gsm
   * Load GSM configuration
   */
  server.on("/api/load/gsm", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    doc["carrierName"] = gsmCfg.carrierName;
    doc["apn"] = gsmCfg.apn;
    doc["apnUser"] = gsmCfg.apnUser;
    doc["apnPass"] = gsmCfg.apnPass;
    
    String out;
    serializeJson(doc, out);
    sendJson(200, out);
  });
  
  /**
   * * POST /api/save/gsm
   * Save GSM configuration
   * Request body: {"carrierName": "...", "apn": "...", "apnUser": "...", "apnPass": "..."}
   */
  server.on("/api/save/gsm", HTTP_POST, []() {
    if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
    
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
    
    gsmCfg.carrierName = doc["carrierName"] | "";
    gsmCfg.apn = doc["apn"] | "";
    gsmCfg.apnUser = doc["apnUser"] | "";
    gsmCfg.apnPass = doc["apnPass"] | "";
    
    bool ok = gsmCfg.save();
    sendText(ok ? 200 : 500, ok ? "OK" : "SAVE_FAILED");
  });
}

// ============================================================================
// SETUP EMAIL DASHBOARD ROUTES
// ============================================================================

/**
 * @brief Setup API endpoints for email configuration dashboard
 * Includes email settings and GSM email sending endpoints
 */
void setupEmailDashboardRoutes() {
  Serial.println("⚙ Setting up EMAIL dashboard routes");
  
  // ============================================================================
  // EMAIL CONFIGURATION ENDPOINTS
  // ============================================================================
  
  /**
   * GET /api/load/email
   * Load email configuration (password excluded for security)
   */
  server.on("/api/load/email", HTTP_GET, []() {
    DynamicJsonDocument doc(512);
    doc["smtpHost"] = emailCfg.smtpHost;
    doc["smtpPort"] = emailCfg.smtpPort;
    doc["emailAccount"] = emailCfg.emailAccount;
    doc["senderName"] = emailCfg.senderName;
    // Note: Password is intentionally excluded for security
    
    String out;
    serializeJson(doc, out);
    sendJson(200, out);
  });
  
  /**
   * POST /api/save/email
   * Save email configuration
   * Request body: {"smtpHost": "...", "smtpPort": 465, "emailAccount": "...", 
   *                "emailPassword": "...", "senderName": "..."}
   */
  server.on("/api/save/email", HTTP_POST, []() {
    if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
    
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
    
    emailCfg.smtpHost = doc["smtpHost"] | emailCfg.smtpHost;
    emailCfg.smtpPort = doc["smtpPort"] | emailCfg.smtpPort;
    emailCfg.emailAccount = doc["emailAccount"] | emailCfg.emailAccount;
    
    // Only update password if provided (allows saving other settings without password)
    if (doc.containsKey("emailPassword")) {
      emailCfg.emailPassword = String((const char*)doc["emailPassword"]);
    }
    
    emailCfg.senderName = doc["senderName"] | emailCfg.senderName;
    
    bool ok = emailCfg.save();
    
    DynamicJsonDocument resp(128);
    resp["success"] = ok;
    
    String out;
    serializeJson(resp, out);
    sendJson(ok ? 200 : 500, out);
  });
  
  // ============================================================================
  // EMAIL SENDING ENDPOINTS
  // ============================================================================
  
  /**
   * POST /api/email/gsm/send
   * Send email via GSM network
   * Request body: {"to": "recipient@example.com", "subject": "...", "content": "..."}
   */
  server.on("/api/email/gsm/send", HTTP_POST, []() {
    if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
    
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
    
    String toEmail = doc["to"] | "";
    String subject = doc["subject"] | "ESP32 GSM Email Test";
    String content = doc["content"] | "This is a test email sent via GSM.";
    
    // Validate recipient email
    if (!toEmail.length()) { sendText(400, "Recipient email required"); return; }
    
    // Send email via GSM
    bool success = sendEmailGSM(toEmail, subject, content);
    
    DynamicJsonDocument resp(256);
    resp["success"] = success;
    
    if (success) {
      resp["message"] = "GSM email sent successfully";
    } else {
      resp["error"] = "Failed to send GSM email";
    }
    
    String out;
    serializeJson(resp, out);
    sendJson(success ? 200 : 500, out);
  });
  
  /**
   * POST /api/email/send?via=gsm
   * Compatibility endpoint for email sending
   * Supports both WiFi and GSM email (currently only GSM implemented)
   * Request body: {"to": "recipient@example.com", "subject": "...", "content": "..."}
   */
  server.on("/api/email/send", HTTP_POST, []() {
    // Check if GSM method is requested
    if (server.hasArg("via") && server.arg("via") == "gsm") {
      // Redirect to GSM send handler
      if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
      
      DynamicJsonDocument doc(1024);
      if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
      
      String toEmail = doc["to"] | "";
      String subject = doc["subject"] | "ESP32 GSM Email Test";
      String content = doc["content"] | "This is a test email sent via GSM.";
      
      if (!toEmail.length()) { sendText(400, "Recipient email required"); return; }
      
      bool success = sendEmailGSM(toEmail, subject, content);
      
      DynamicJsonDocument resp(256);
      resp["success"] = success;
      
      if (success) {
        resp["message"] = "GSM email sent successfully";
      } else {
        resp["error"] = "Failed to send GSM email";
      }
      
      String out;
      serializeJson(resp, out);
      sendJson(success ? 200 : 500, out);
    } else {
      // WiFi email not yet implemented
      sendText(501, "WiFi email not implemented");
    }
  });
}

// ============================================================================
// SETUP
// ============================================================================

/**
 * @brief Main setup function
 * Initializes all hardware and software components
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  
  // ============================================================================
  // DOUBLE RESET DETECTION
  // ============================================================================
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  ESP32 Configuration Panel with DRD   ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Detect double reset to determine dashboard mode
  bool doubleResetDetected = drd.detectDoubleReset();
  
  if (doubleResetDetected) {
    currentMode = MODE_EMAIL;
    Serial.println("⚡ DOUBLE RESET DETECTED!");
    Serial.println("📧 Loading EMAIL Configuration Dashboard");
  } else {
    currentMode = MODE_MAIN;
    Serial.println("🔄 Single reset detected");
    Serial.println("📊 Loading MAIN Dashboard");
  }
  Serial.println("────────────────────────────────────────");
  
  // ============================================================================
  // FILESYSTEM INITIALIZATION
  // ============================================================================
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS mount failed");
  } else {
    Serial.println("✅ SPIFFS mounted successfully");
  }
  
  // ============================================================================
  // LOAD CONFIGURATIONS
  // ============================================================================
  wifiCfg.load();
  gsmCfg.load();
  userCfg.load();
  emailCfg.load();
  
  Serial.println("\n📋 Configuration Status:");
  Serial.printf("  WiFi AP: %s\n", wifiCfg.apSsid.length() ? wifiCfg.apSsid.c_str() : DEFAULT_AP_SSID);
  Serial.printf("  WiFi STA: %s\n", wifiCfg.staSsid.length() ? wifiCfg.staSsid.c_str() : "Not configured");
  Serial.printf("  GSM APN: %s\n", gsmCfg.apn.length() ? gsmCfg.apn.c_str() : "Not configured");
  Serial.printf("  Email: %s\n", emailCfg.isValid() ? emailCfg.emailAccount.c_str() : "Not configured");
  
  // ============================================================================
  // WIFI ACCESS POINT
  // ============================================================================
  String apSsid = wifiCfg.apSsid.length() ? wifiCfg.apSsid : DEFAULT_AP_SSID;
  String apPass = wifiCfg.apPass.length() ? wifiCfg.apPass : DEFAULT_AP_PASS;
  startAP(apSsid, apPass);
  
  Serial.println("\n📡 Access Point Started:");
  Serial.printf("  SSID: %s\n", apSsid.c_str());
  Serial.printf("  IP: %s\n", ipToStr(WiFi.softAPIP()).c_str());
  Serial.printf("  Mode: %s\n", (currentMode == MODE_MAIN) ? "MAIN Dashboard" : "EMAIL Dashboard");
  
  // ============================================================================
  // WIFI STATION MODE
  // ============================================================================
  if (wifiCfg.staSsid.length()) {
    Serial.printf("\n🔌 Connecting to: %s\n", wifiCfg.staSsid.c_str());
    connectSTA(wifiCfg.staSsid, wifiCfg.staPass);
  }
  
  // ============================================================================
  // GSM INITIALIZATION (Only for MAIN mode)
  // ============================================================================
  if (currentMode == MODE_MAIN) {
    Serial.println("\n📱 Initializing GSM modem...");
    gsmModem.begin();
    delay(2000);  // Allow modem to stabilize
    Serial.println("✅ GSM modem initialized");
  }
  
  // ============================================================================
  // WEB SERVER SETUP
  // ============================================================================
  Serial.println("\n🌐 Setting up web server...");
  
  // ============================================================================
  // COMMON ROUTES
  // ============================================================================
  
  // Main dashboard pages
  server.on("/", HTTP_GET, handleRoot);
  server.on("/index.html", HTTP_GET, handleRoot);
  server.on("/config", HTTP_GET, handleRoot);
  
  // ============================================================================
  // CAPTIVE PORTAL ENDPOINTS
  // ============================================================================
  // These endpoints handle various OS captive portal detection mechanisms
  
  // Android captive portal detection
  server.on("/generate_204", HTTP_GET, []() {
    addCORS();
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302, "text/plain", "");
  });
  
  // Windows captive portal detection
  server.on("/ncsi.txt", HTTP_GET, []() {
    addCORS();
    server.send(200, "text/plain", "Microsoft NCSI");
  });
  
  // iOS/macOS captive portal detection
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  
  // Generic success page
  server.on("/success.txt", HTTP_GET, []() {
    addCORS();
    server.send(200, "text/plain", "success");
  });
  
  // ============================================================================
  // MODE MANAGEMENT ENDPOINTS
  // ============================================================================
  server.on("/api/mode", HTTP_GET, handleSwitchMode);
  server.on("/api/mode/switch", HTTP_POST, handleModeSwitchRequest);
  
  // ============================================================================
  // COMMON STATUS ENDPOINT
  // ============================================================================
  server.on("/api/status", HTTP_GET, []() { 
    sendJson(200, buildStatusJson()); 
  });
  
  // ============================================================================
  // SETUP MODE-SPECIFIC ROUTES
  // ============================================================================
  if (currentMode == MODE_MAIN) {
    setupMainDashboardRoutes();
    Serial.println("✅ Main dashboard routes configured");
  } else {
    setupEmailDashboardRoutes();
    Serial.println("✅ Email dashboard routes configured");
  }
  
  // ============================================================================
  // CORS OPTIONS HANDLERS
  // ============================================================================
  // Handle preflight OPTIONS requests for all API endpoints
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/mode", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/scan", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/connect", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/disconnect", HTTP_OPTIONS, handleOptions);
  server.on("/api/gsm/signal", HTTP_OPTIONS, handleOptions);
  server.on("/api/gsm/network", HTTP_OPTIONS, handleOptions);
  server.on("/api/gsm/call", HTTP_OPTIONS, handleOptions);
  server.on("/api/gsm/call/hangup", HTTP_OPTIONS, handleOptions);
  server.on("/api/gsm/sms", HTTP_OPTIONS, handleOptions);
  server.on("/api/load/user", HTTP_OPTIONS, handleOptions);
  server.on("/api/save/user", HTTP_OPTIONS, handleOptions);
  server.on("/api/load/gsm", HTTP_OPTIONS, handleOptions);
  server.on("/api/save/gsm", HTTP_OPTIONS, handleOptions);
  server.on("/api/load/email", HTTP_OPTIONS, handleOptions);
  server.on("/api/save/email", HTTP_OPTIONS, handleOptions);
  server.on("/api/email/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/email/gsm/send", HTTP_OPTIONS, handleOptions);
  
  // ============================================================================
  // ERROR HANDLERS
  // ============================================================================
  server.on("/", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);
  
  // ============================================================================
  // START SERVER
  // ============================================================================
  server.begin();
  Serial.println("✅ HTTP server started");
  
  // ============================================================================
  // STARTUP COMPLETE
  // ============================================================================
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║         SYSTEM READY                   ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.printf("🌐 Portal URL: http://%s\n", ipToStr(WiFi.softAPIP()).c_str());
  Serial.printf("📊 Dashboard Mode: %s\n", (currentMode == MODE_MAIN) ? "MAIN" : "EMAIL");
  Serial.println("\n💡 TIP: Double-reset within 3 seconds to switch dashboards");
  Serial.println("────────────────────────────────────────\n");
}

// ============================================================================
// LOOP
// ============================================================================

/**
 * @brief Main loop function
 * Handles ongoing network operations and periodic status updates
 */
void loop() {
  // ============================================================================
  // NETWORK HANDLING
  // ============================================================================
  dnsServer.processNextRequest();  // Handle DNS requests for captive portal
  server.handleClient();            // Handle HTTP requests
  
  // ============================================================================
  // DOUBLE RESET DETECTION
  // ============================================================================
  drd.loop();  // Auto-clear DRD flag after timeout
  
  // ============================================================================
  // PERIODIC STATUS LOGGING
  // ============================================================================
  static unsigned long lastStatusPrint = 0;
  const unsigned long STATUS_INTERVAL = 30000;  // 30 seconds
  
  if (millis() - lastStatusPrint > STATUS_INTERVAL) {
    lastStatusPrint = millis();
    
    // Print status header
    Serial.printf("\n📊 Status Update [%s mode]\n", 
                  (currentMode == MODE_MAIN) ? "MAIN" : "EMAIL");
    
    // Access Point status
    Serial.printf("  AP IP: %s\n", ipToStr(WiFi.softAPIP()).c_str());
    Serial.printf("  Connected devices: %d\n", WiFi.softAPgetStationNum());
    
    // Station mode status
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("  STA IP: %s\n", ipToStr(WiFi.localIP()).c_str());
      Serial.printf("  RSSI: %d dBm (%s)\n", WiFi.RSSI(), rssiToStrength(WiFi.RSSI()));
    } else {
      Serial.println("  STA: Not connected");
    }
    
    // Email configuration status (only in EMAIL mode)
    if (currentMode == MODE_EMAIL) {
      Serial.printf("  Email: %s\n", emailCfg.isValid() ? "Configured ✓" : "Not configured ✗");
    }
    
    // GSM status (only in MAIN mode)
    if (currentMode == MODE_MAIN) {
      if (gsmCache.signalStrength != 0) {
        Serial.printf("  GSM Signal: %d dBm (%s)\n", 
                      gsmCache.signalStrength, 
                      gsmCache.grade.c_str());
        Serial.printf("  GSM Carrier: %s\n", gsmCache.carrierName.c_str());
      } else {
        Serial.println("  GSM: Not initialized");
      }
    }
    
    Serial.println("────────────────────────────────────────");
  }
}
