#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "SMTP.h"           // <- New SMTP library
#include "config_html.h"
//
// ============================================================================
// NETWORK / SERVER SETTINGS
// ============================================================================
#define DNS_PORT 53
#define WIFI_AP_IP IPAddress(192,168,4,1)

static const char* EMAIL_FILE = "/email.json";
static const char* DEFAULT_AP_SSID = "ESP32-AccessPoint";
static const char* DEFAULT_AP_PASS = "12345678";
static const char* DEFAULT_SMTP_HOST = "smtp.gmail.com";
static const int   DEFAULT_SMTP_PORT = 465;
static const char* DEFAULT_SENDER_NAME = "ESP32 Device";

// ============================================================================
// INSTANCES
// ============================================================================
DNSServer dnsServer;
WebServer server(80);
SMTP smtp(Serial2, 16, 17, 115200);  // RX=16, TX=17

String apSsid = DEFAULT_AP_SSID;
String apPass = DEFAULT_AP_PASS;

// ============================================================================
// EMAIL CONFIG STRUCT
// ============================================================================
struct EmailConfig {
  String smtpHost = DEFAULT_SMTP_HOST;
  int smtpPort = DEFAULT_SMTP_PORT;
  String emailAccount = "";
  String emailPassword = "";
  String senderName = DEFAULT_SENDER_NAME;

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

  bool isValid() const {
    return smtpHost.length() > 0 && emailAccount.length() > 0 && emailPassword.length() > 0;
  }
} emailCfg;

// ============================================================================
// UTILITIES
// ============================================================================
String ipToStr(const IPAddress &ip) {
  char buf[24];
  snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  return String(buf);
}

void addCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Cache-Control", "no-store");
}

void sendJson(int code, const String& body) {
  addCORS();
  server.send(code, "application/json", body);
}

void sendText(int code, const String& body, const String& ctype = "text/plain") {
  addCORS();
  server.send(code, ctype, body);
}

// ============================================================================
// ACCESS POINT
// ============================================================================
void startAP(const String& ssid, const String& pass) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, IPAddress(255,255,255,0));
  bool ap_ok;
  if (pass.length() >= 8)
    ap_ok = WiFi.softAP(ssid.c_str(), pass.c_str());
  else
    ap_ok = WiFi.softAP(ssid.c_str());
  if (!ap_ok) WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS);
  delay(500);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

// ============================================================================
// GSM EMAIL FUNCTION VIA SMTP LIBRARY
// ============================================================================
bool sendEmailGSM(const String& toEmail, const String& subject, const String& content) {
  Serial.println("📡 Sending email via GSM (SMTP library)...");
  if (!emailCfg.isValid()) {
    Serial.println("❌ Email configuration incomplete");
    return false;
  }

  smtp.begin();
  smtp.setAPN("internet"); // Use default APN or you can make dynamic
  smtp.setAuth(emailCfg.emailAccount.c_str(), emailCfg.emailPassword.c_str());
  smtp.setRecipient(toEmail.c_str());
  smtp.setFromName(emailCfg.senderName.c_str());
  smtp.setSubject(subject);
  smtp.setBody(content);

  bool success = smtp.sendEmail();
  return success;
}

// ============================================================================
// HTTP HANDLERS
// ============================================================================
void handleRoot() {
  addCORS();
  server.send_P(200, "text/html", config_html, config_html_len);
}

void handleOptions() {
  addCORS();
  server.send(204);
}

void handleNotFound() {
  addCORS();
  server.send_P(200, "text/html", config_html, config_html_len);
}

void handleEmailLoad() {
  addCORS();
  DynamicJsonDocument doc(512);
  doc["smtpHost"] = emailCfg.smtpHost;
  doc["smtpPort"] = emailCfg.smtpPort;
  doc["emailAccount"] = emailCfg.emailAccount;
  doc["senderName"] = emailCfg.senderName;
  String out; serializeJson(doc, out);
  sendJson(200, out);
}

void handleEmailSave() {
  if (!server.hasArg("plain")) { sendText(400, "Invalid JSON"); return; }
  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, server.arg("plain"))) { sendText(400, "Invalid JSON"); return; }
  emailCfg.smtpHost = doc["smtpHost"] | emailCfg.smtpHost;
  emailCfg.smtpPort = doc["smtpPort"] | emailCfg.smtpPort;
  emailCfg.emailAccount = doc["emailAccount"] | emailCfg.emailAccount;
  if (doc.containsKey("emailPassword")) emailCfg.emailPassword = String((const char*)doc["emailPassword"]);
  emailCfg.senderName = doc["senderName"] | emailCfg.senderName;
  bool ok = emailCfg.save();
  DynamicJsonDocument resp(128);
  resp["success"] = ok;
  String out; serializeJson(resp, out);
  sendJson(ok ? 200 : 500, out);
}

void handleGsmEmailSend() {
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
  if (success) resp["message"] = "GSM email sent successfully";
  else resp["error"] = "Failed to send GSM email";
  String out; serializeJson(resp, out);
  sendJson(success ? 200 : 500, out);
}

void handleEmailSendCompat() {
  if (server.hasArg("via") && server.arg("via") == "gsm") {
    handleGsmEmailSend();
    return;
  }
  sendText(501, "WiFi email not implemented");
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== ESP32 GSM CONFIG PANEL ===");

  if (!SPIFFS.begin(true)) Serial.println("⚠️ SPIFFS mount failed");
  else Serial.println("✅ SPIFFS mounted");

  emailCfg.load();

  startAP(apSsid, apPass);
  Serial.println("📶 Access Point started:");
  Serial.print("SSID: "); Serial.println(apSsid);
  Serial.print("IP: "); Serial.println(ipToStr(WiFi.softAPIP()));

  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  delay(100);

  // Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/index.html", HTTP_GET, handleRoot);
  server.on("/config", HTTP_GET, handleRoot);
  server.on("/generate_204", HTTP_GET, handleRoot);
  server.on("/ncsi.txt", HTTP_GET, [](){ addCORS(); server.send(200, "text/plain", "Microsoft NCSI"); });
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  server.on("/success.txt", HTTP_GET, handleRoot);

  server.on("/api/load/email", HTTP_GET, handleEmailLoad);
  server.on("/api/save/email", HTTP_POST, handleEmailSave);
  server.on("/api/email/gsm/send", HTTP_POST, handleGsmEmailSend);
  server.on("/api/email/send", HTTP_POST, handleEmailSendCompat);
  server.on("/api/email/send", HTTP_OPTIONS, handleOptions);

  server.on("/", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("🌐 HTTP server started");
  Serial.print("Portal URL: http://");
  Serial.println(ipToStr(WiFi.softAPIP()));
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
