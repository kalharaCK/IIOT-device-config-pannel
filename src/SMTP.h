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
