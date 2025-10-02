/* ---------------------------------------------------------------------------
   SMTP.cpp
   Implementation of the SMTP helper for sending emails via A76xx modules.

   Features:
     - Bring up PDP context with APN
     - Start TLS/SSL channel for SMTP
     - Authenticate with Gmail using LOGIN (Base64)
     - Compose and send email with subject/body
     - Proper teardown (QUIT, close channels)

--------------------------------------------------------------------------- */

#include "SMTP.h"

static const int LINK_ID = 0;

// Constructor: store UART reference and pin config
SMTP::SMTP(HardwareSerial& modem, int rxPin, int txPin, long baud)
: _m(modem), _rxPin(rxPin), _txPin(txPin), _baud(baud) {}

void SMTP::begin() {
  _m.begin(_baud, SERIAL_8N1, _rxPin, _txPin);
}

// === Config setters ===
void SMTP::setAPN(const char* apn) { _apn = apn ? apn : ""; }
void SMTP::setAuth(const char* g, const char* p) { _gmail = g ? g : ""; _appPass = p ? p : ""; }
void SMTP::setRecipient(const char* to, const char* name) { _to = to ? to : ""; _toName = name ? name : ""; }
void SMTP::setFromName(const char* name) { _fromName = name ? name : ""; }
void SMTP::setSubject(const String& subject) { _subject = subject; }
void SMTP::setBody(const String& body) { _body = body; }

// === Helpers: AT command wrappers ===
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
  _m.print(cmd); _m.print("\r\n");
  return waitFor(expect, ms);
}

bool SMTP::ATAcceptAny(const String& cmd, const String* tokens, size_t ntokens, uint32_t ms) {
#if SMTP_DEBUG
  Serial.print(F(">>> ")); Serial.println(cmd);
#endif
  _m.print(cmd); _m.print("\r\n");
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

// === PDP Context ===
bool SMTP::bringUpPDP() {
  AT("ATE0"); AT("AT+CMEE=2");
  if (_apn.length()) {
    AT(String("AT+CGDCONT=1,\"IP\",\"") + _apn + "\"");
    AT("AT+CSOCKSETPN=1");
  }
  const String okTokens[] = { String("OK"), String("+NETOPEN: 0"), String("+IP ERROR: Network is already opened"), String("already opened") };
  return ATAcceptAny("AT+NETOPEN", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 20000);
}

void SMTP::tearDownPDP() {
  const String okTokens[] = { String("OK"), String("+NETCLOSE: 0"), String("ERROR") };
  ATAcceptAny("AT+NETCLOSE", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 10000);
}

// === SSL/TLS channel control ===
bool SMTP::cchStart() {
  AT("AT+CSSLCFG=\"sslversion\",0,3");
  AT("AT+CSSLCFG=\"authmode\",0,0");
  AT("AT+CSSLCFG=\"ignorelocaltime\",0,1");
  const String okTokens[] = { String("OK"), String("ERROR") };
  return ATAcceptAny("AT+CCHSTART", okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 8000);
}

bool SMTP::cchOpen(const char* host, uint16_t port, int link) {
  String cmd = String("AT+CCHOPEN=") + link + ",\"" + host + "\"," + port;
  const String okTokens[] = { String("OK"), String("+CCHOPEN"), String("ERROR") };
  bool ok = ATAcceptAny(cmd, okTokens, sizeof(okTokens)/sizeof(okTokens[0]), 15000);
  delay(200);
  return ok;
}

bool SMTP::cchSendRaw(int link, const uint8_t* data, size_t len) {
  String cmd = String("AT+CCHSEND=") + link + "," + (int)len;
#if SMTP_DEBUG
  Serial.print(F(">>> ")); Serial.println(cmd);
#endif
  _m.print(cmd); _m.print("\r\n");
  if (!waitFor(">", 8000)) return false;
  _m.write(data, len);
  return waitFor("OK", 8000);
}

bool SMTP::cchSendLine(int link, const String& line) {
  String s = line + "\r\n";
  return cchSendRaw(link, (const uint8_t*)s.c_str(), s.length());
}

bool SMTP::cchRecvChunk(int link, String& out, uint16_t maxBytes, uint32_t ms) {
  out = "";
  uint32_t t0 = millis();
  uint32_t nextPoll = millis();
  const uint32_t POLL_INTERVAL_MS = 300;
  while (millis() - t0 < ms) {
    while (_m.available()) out += (char)_m.read();
    if (out.indexOf("+CCHRECV: DATA,") >= 0) {
      delay(20);
      while (_m.available()) out += (char)_m.read();
      return true;
    }
    if (millis() >= nextPoll) {
      nextPoll = millis() + POLL_INTERVAL_MS;
      String cmd = String("AT+CCHRECV=") + link + "," + maxBytes;
      _m.print(cmd); _m.print("\r\n");
      uint32_t t1 = millis();
      while (millis() - t1 < 150) if (_m.available()) out += (char)_m.read();
      if (out.indexOf("+CCHRECV: DATA,") >= 0) {
        delay(20);
        while (_m.available()) out += (char)_m.read();
        return true;
      } else {
        out = "";
      }
    }
    delay(10); yield();
  }
  return false;
}

void SMTP::cchClose(int link) { AT(String("AT+CCHCLOSE=") + link); }
void SMTP::cchStop()          { AT("AT+CCHSTOP"); }

// === Base64 for SMTP AUTH ===
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

// === SMTP Protocol flow ===
bool SMTP::smtpExpect(int link, const char* code, uint32_t ms) {
  String chunk;
  if (!cchRecvChunk(link, chunk, 512, ms)) return false;
  int start = 0;
  while (true) {
    int end = chunk.indexOf("\r\n", start);
    String line = (end >= 0) ? chunk.substring(start, end) : chunk.substring(start);
    String t = line; t.trim();
    if (t.length()) {
#if SMTP_DEBUG
      Serial.print("[SMTP] "); Serial.println(t);
#endif
      if (t.startsWith(code)) return true;
    }
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
  if (!cchSendLine(link, String("MAIL FROM:<") + _gmail + ">")) return false;
  if (!smtpExpect(link, "250", 8000)) return false;
  if (!cchSendLine(link, String("RCPT TO:<") + _to + ">")) return false;
  if (!smtpExpect(link, "250", 8000)) return false;
  if (!cchSendLine(link, "DATA")) return false;
  if (!smtpExpect(link, "354", 8000)) return false;
  {
    String fromN = _fromName.length() ? _fromName : "ESP32";
    String toN   = _toName.length()   ? _toName   : "Recipient";
    String subj  = _subject.length()  ? _subject  : "No Subject";
    String payload;
    payload.reserve(256 + _body.length());
    payload += "From: " + fromN + " <" + _gmail + ">\r\n";
    payload += "To: "   + toN   + " <" + _to    + ">\r\n";
    payload += "Subject: " + subj + "\r\n";
    payload += "MIME-Version: 1.0\r\n";
    payload += "Content-Type: text/plain; charset=UTF-8\r\n";
    payload += "\r\n";
    payload += _body;
    payload += "\r\n.\r\n";
    if (!cchSendRaw(link, (const uint8_t*)payload.c_str(), payload.length())) return false;
    if (!smtpExpect(link, "250", 12000)) return false;
  }
  cchSendLine(link, "QUIT");
  smtpExpect(link, "221", 5000);
  return true;
}

// === Public API ===
bool SMTP::sendEmail() {
  if (_gmail.isEmpty() || _appPass.isEmpty() || _to.isEmpty()) return false;
  if (!bringUpPDP()) return false;
  if (!cchStart()) { tearDownPDP(); return false; }
  if (!cchOpen("smtp.gmail.com", 465, LINK_ID)) { cchStop(); tearDownPDP(); return false; }
  bool ok = smtpSession(LINK_ID);
  cchClose(LINK_ID);
  cchStop();
  tearDownPDP();
  return ok;
}

void SMTP::bridge(Stream& usb) {
  while (usb.available()) _m.write(usb.read());
  while (_m.available())  usb.write(_m.read());
}
