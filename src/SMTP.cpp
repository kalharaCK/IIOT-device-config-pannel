/* ---------------------------------------------------------------------------
   SMTP.cpp - Patched version
   Robust GSM Email via SIMCom A76xx
--------------------------------------------------------------------------- */

#include "SMTP.h"

static const int LINK_ID = 0;
#define MAX_RETRIES 3
#define CRLF "\r\n"

// ---------------- Constructor ----------------
SMTP::SMTP(HardwareSerial& modem, int rxPin, int txPin, long baud)
: _m(modem), _rxPin(rxPin), _txPin(txPin), _baud(baud) {}

void SMTP::begin() {
  _m.begin(_baud, SERIAL_8N1, _rxPin, _txPin);
}

// ---------------- Setters ----------------
void SMTP::setAPN(const char* apn) { _apn = apn ? apn : ""; }
void SMTP::setAuth(const char* g, const char* p) { _gmail = g ? g : ""; _appPass = p ? p : ""; }
void SMTP::setRecipient(const char* to, const char* name) { _to = to ? to : ""; _toName = name ? name : ""; }
void SMTP::setFromName(const char* name) { _fromName = name ? name : ""; }
void SMTP::setSubject(const String& subject) { _subject = subject; }
void SMTP::setBody(const String& body) {
  // Normalize line endings to CRLF
  _body = body;
  _body.replace("\n", CRLF);
  _body.replace("\r" CRLF, CRLF); // avoid double CRLF
}

// ---------------- AT Wrappers ----------------
bool SMTP::waitFor(const String& token, uint32_t ms) {
  String buf; uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    while (_m.available()) {
      char c = _m.read();
#if SMTP_DEBUG
      Serial.write(c);
#endif
      buf += c;
      if (buf.indexOf(token) >= 0) return true;
    }
    yield();
  }
  return false;
}

bool SMTP::readUntil(String& out, const String& token, uint32_t ms) {
  out = ""; uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    while (_m.available()) {
      char c = _m.read();
#if SMTP_DEBUG
      Serial.write(c);
#endif
      out += c;
      if (out.indexOf(token) >= 0) return true;
    }
    yield();
  }
  return false;
}

bool SMTP::AT(const String& cmd, const String& expect, uint32_t ms) {
#if SMTP_DEBUG
  Serial.print(F(">>> ")); Serial.println(cmd);
#endif
  _m.print(cmd); _m.print(CRLF);
  return waitFor(expect, ms);
}

bool SMTP::ATAcceptAny(const String& cmd, const String* tokens, size_t ntokens, uint32_t ms) {
#if SMTP_DEBUG
  Serial.print(F(">>> ")); Serial.println(cmd);
#endif
  _m.print(cmd); _m.print(CRLF);
  String buf; uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    while (_m.available()) {
      char c = _m.read();
#if SMTP_DEBUG
      Serial.write(c);
#endif
      buf += c;
      for (size_t i=0;i<ntokens;i++) {
        if (!tokens[i].isEmpty() && buf.indexOf(tokens[i]) >= 0) return true;
      }
    }
    yield();
  }
  return false;
}

// ---------------- PDP Context ----------------
bool SMTP::bringUpPDP() {
  for (int attempt=0; attempt<MAX_RETRIES; attempt++) {
    AT("ATE0"); AT("AT+CMEE=2");
    if (_apn.length()) {
      AT("AT+CGDCONT=1,\"IP\",\"" + _apn + "\"");
      AT("AT+CSOCKSETPN=1");
    }
    const String okTokens[] = { "OK", "+NETOPEN: 0", "already opened" };
    if (ATAcceptAny("AT+NETOPEN", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 20000)) {
      delay(1000);
      return true;
    }
    Serial.println(" PDP open failed, retrying...");
    delay(2000);
  }
  return false;
}

void SMTP::tearDownPDP() {
  const String okTokens[] = { "OK", "+NETCLOSE: 0", "ERROR" };
  ATAcceptAny("AT+NETCLOSE", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 10000);
}

// ---------------- SSL/TLS ----------------
bool SMTP::cchStart() {
  for (int attempt=0; attempt<MAX_RETRIES; attempt++) {
    AT("AT+CSSLCFG=\"sslversion\",0,3");
    AT("AT+CSSLCFG=\"authmode\",0,0");
    AT("AT+CSSLCFG=\"ignorelocaltime\",0,1");
    const String okTokens[] = { "OK", "ERROR" };
    if (ATAcceptAny("AT+CCHSTART", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 10000)) {
      delay(500);
      return true;
    }
    Serial.println(" CCHSTART failed, retrying...");
    delay(2000);
  }
  return false;
}

bool SMTP::cchOpen(const char* host, uint16_t port, int link) {
  String cmd = "AT+CCHOPEN=" + String(link) + ",\"" + host + "\"," + port;
  const String okTokens[] = { "OK", "+CCHOPEN", "ERROR" };
  bool ok = ATAcceptAny(cmd, okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 15000);
  delay(500);
  return ok;
}

bool SMTP::cchSendRaw(int link, const uint8_t* data, size_t len) {
  String cmd = "AT+CCHSEND=" + String(link) + "," + (int)len;
#if SMTP_DEBUG
  Serial.print(F(">>> ")); Serial.println(cmd);
#endif
  _m.print(cmd); _m.print(CRLF);
  if (!waitFor(">", 8000)) return false;
  _m.write(data, len);
  return waitFor("OK", 8000);
}

bool SMTP::cchSendLine(int link, const String& line) {
  return cchSendRaw(link, (const uint8_t*)(line + CRLF).c_str(), line.length() + 2);
}

bool SMTP::cchRecvChunk(int link, String& out, uint16_t maxBytes, uint32_t ms) {
  out = "";
  uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    while (_m.available()) out += (char)_m.read();
    if (out.indexOf("+CCHRECV: DATA,") >= 0) {
      delay(20);
      while (_m.available()) out += (char)_m.read();
      return true;
    }
    delay(10); yield();
  }
  return false;
}

void SMTP::cchClose(int link) { AT("AT+CCHCLOSE=" + String(link)); }
void SMTP::cchStop()          { AT("AT+CCHSTOP"); }

// ---------------- Base64 ----------------
static const char* B64ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
String SMTP::b64(const String& in) {
  String out; int val=0, valb=-6;
  for (uint8_t c : in) {
    val = (val<<8) + c; valb += 8;
    while (valb >= 0) { out += B64ABC[(val>>valb) & 0x3F]; valb -= 6; }
  }
  if (valb > -6) out += B64ABC[((val<<8)>>(valb+8)) & 0x3F];
  while (out.length() % 4) out += '=';
  return out;
}

// ---------------- SMTP Flow ----------------
bool SMTP::smtpExpect(int link, const char* code, uint32_t ms) {
  String chunk;
  if (!cchRecvChunk(link, chunk, 512, ms)) return false;
  int start = 0;
  while (true) {
    int end = chunk.indexOf(CRLF, start);
    String line = (end >= 0) ? chunk.substring(start, end) : chunk.substring(start);
    line.trim();
    if (line.length() && line.startsWith(code)) return true;
    if (end < 0) break;
    start = end + 2;
  }
  return false;
}

bool SMTP::smtpSession(int link) {
  if (!smtpExpect(link, "220", 15000)) return false;
  if (!cchSendLine(link, "EHLO simcom")) return false;
  if (!smtpExpect(link, "250", 10000)) return false;
  if (!cchSendLine(link, "AUTH LOGIN")) return false;
  if (!smtpExpect(link, "334", 8000)) return false;
  if (!cchSendLine(link, b64(_gmail))) return false;
  if (!smtpExpect(link, "334", 8000)) return false;
  if (!cchSendLine(link, b64(_appPass))) return false;
  if (!smtpExpect(link, "235", 10000)) return false;
  if (!cchSendLine(link, "MAIL FROM:<" + _gmail + ">")) return false;
  if (!smtpExpect(link, "250", 8000)) return false;
  if (!cchSendLine(link, "RCPT TO:<" + _to + ">")) return false;
  if (!smtpExpect(link, "250", 8000)) return false;
  if (!cchSendLine(link, "DATA")) return false;
  if (!smtpExpect(link, "354", 8000)) return false;

  String payload;
  payload.reserve(256 + _body.length());
  payload += "From: " + (_fromName.length() ? _fromName : "ESP32") + " <" + _gmail + ">" CRLF;
  payload += "To: " + (_toName.length() ? _toName : "Recipient") + " <" + _to + ">" CRLF;
  payload += "Subject: " + (_subject.length() ? _subject : "No Subject") + CRLF;
  payload += "MIME-Version: 1.0" CRLF;
  payload += "Content-Type: text/plain; charset=UTF-8" CRLF CRLF;
  payload += _body + CRLF ".\r\n";

  if (!cchSendRaw(link, (const uint8_t*)payload.c_str(), payload.length())) return false;
  if (!smtpExpect(link, "250", 12000)) return false;

  cchSendLine(link, "QUIT");
  smtpExpect(link, "221", 5000);
  return true;
}

// ---------------- Public API ----------------
bool SMTP::sendEmail() {
  if (_gmail.isEmpty() || _appPass.isEmpty() || _to.isEmpty()) return false;

  Serial.println("📡 Bringing up PDP...");
  if (!bringUpPDP()) { Serial.println(" PDP failed"); return false; }

  Serial.println(" Starting SSL/TLS...");
  if (!cchStart()) { Serial.println("SSL failed"); tearDownPDP(); return false; }

  Serial.println(" Opening SMTP connection...");
  if (!cchOpen("smtp.gmail.com", 465, LINK_ID)) {
    Serial.println(" SMTP connect failed"); cchStop(); tearDownPDP(); return false;
  }

  Serial.println(" Sending SMTP session...");
  bool ok = smtpSession(LINK_ID);
  cchClose(LINK_ID);
  cchStop();
  tearDownPDP();

  if (ok) Serial.println(" Email sent successfully!");
  else Serial.println(" Email send failed!");
  return ok;
}
  
// ---------------- Bridge ----------------
void SMTP::bridge(Stream& usb) {
  while (usb.available()) _m.write(usb.read());
  while (_m.available())  usb.write(_m.read());
}
