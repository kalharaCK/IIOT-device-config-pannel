/**
 * @file GSM_Email_Test_Example.cpp
 * @brief Example code demonstrating GSM email functionality
 * 
 * This example shows how to use the GSM email functionality alongside WiFi email.
 * Both use the same SMTP credentials but route through different connections.
 * 
 * @details
 * GSM Email Setup:
 * 1. Uses APN="internet" as specified
 * 2. Shares same SMTP credentials as WiFi email
 * 3. Routes through GSM data connection instead of WiFi
 * 4. Uses SIMCom A76xx modules built-in SMTP client
 * 
 * Wiring:
 * - ESP32 RX2 (GPIO 16) -> GSM Modem TX
 * - ESP32 TX2 (GPIO 17) -> GSM Modem RX
 * - Common GND connection
 * - Power supply for modem
 * 
 * Requirements:
 * - GSM modem with active SIM card
 * - Valid APN configuration (default: "internet")
 * - Configured SMTP settings (same as WiFi email)
 */

#include <Arduino.h>
#include "GSM_Test.h"

// GSM modem instance
GSM_Test gsmModem(Serial2, 16, 17, 115200);

// Email configuration (same as WiFi email settings)
const char* SMTP_HOST = "smtp.gmail.com";
const int SMTP_PORT = 465;
const char* EMAIL_ACCOUNT = "your-email@gmail.com";      // Replace with your Gmail
const char* APP_PASSWORD = "your-app-password";          // Replace with your Gmail App Password
const char* SENDER_NAME = "GSM Device";

// Test recipient email
const char* TEST_RECIPIENT = "recipient@example.com";

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== GSM Email Test Example ===");
  Serial.println("This example demonstrates GSM email functionality");
  Serial.println("");
  
  // Initialize GSM modem
  Serial.println("1. Initializing GSM modem...");
  gsmModem.begin();
  delay(3000);  // Give modem time to initialize
  
  // Check SIM card status
  Serial.println("2. Checking SIM card status...");
  if (!gsmModem.checkSIM()) {
    Serial.println("   ERROR: SIM card not ready!");
    Serial.println("   Please check SIM card insertion and activation.");
    return;
  }
  Serial.println("   ✓ SIM card is ready");
  
  // Initialize GSM SMTP client with APN="internet"
  Serial.println("3. Initializing GSM SMTP client...");
  if (!gsmModem.initSMTP("internet")) {
    Serial.println("   ERROR: Failed to initialize GSM SMTP client!");
    Serial.println("   Please check GSM network registration.");
    return;
  }
  Serial.println("   ✓ GSM SMTP client initialized with APN: internet");
  
  // Configure SMTP settings (same credentials as WiFi email)
  Serial.println("4. Configuring SMTP settings...");
  if (!gsmModem.configSMTP(SMTP_HOST, SMTP_PORT, EMAIL_ACCOUNT, APP_PASSWORD, SENDER_NAME)) {
    Serial.println("   ERROR: Failed to configure SMTP!");
    Serial.println("   Please check your SMTP credentials.");
    return;
  }
  Serial.println("   ✓ SMTP configuration completed");
  Serial.println("      Host: " + String(SMTP_HOST) + ":" + String(SMTP_PORT));
  Serial.println("      Account: " + String(EMAIL_ACCOUNT));
  Serial.println("      Sender: " + String(SENDER_NAME));
  
  Serial.println("");
  Serial.println("=== GSM Email Test Setup Complete ===");
  Serial.println("GSM modem ready for email operations!");
  Serial.println("You can now send emails via GSM data connection.");
  Serial.println("");
}

void loop() {
  // Example: Send a test email via GSM
  Serial.println("=== Sending Test Email via GSM ===");
  
  String subject = "Test Email via GSM from ESP32";
  String body = "Hello!\n\n";
  body += "This is a test email sent via GSM from my ESP32 device.\n\n";
  body += "GSM Email Setup:\n";
  body += "- APN: internet (as requested)\n";
  body += "- Same SMTP credentials as WiFi email\n";
  body += "- Routes through GSM data connection\n";
  body += "- Uses SIMCom A76xx built-in SMTP client\n\n";
  body += "Email sent successfully via GSM!";
  
  Serial.println("Recipient: " + String(TEST_RECIPIENT));
  Serial.println("Subject: " + subject);
  Serial.println("Sending email...");
  
  bool success = gsmModem.sendEmailViaGSM(TEST_RECIPIENT, subject, body);
  
  if (success) {
    Serial.println("✓ Email sent successfully via GSM!");
  } else {
    Serial.println("✗ Email failed to send via GSM");
    Serial.println("Possible causes:");
    Serial.println("- GSM network not registered");
    Serial.println("- Insufficient signal strength");
    Serial.println("- SMTP authentication failed");
    Serial.println("- APN configuration issues");
  }
  
  Serial.println("");
  Serial.println("Waiting 60 seconds before next test...");
  delay(60000);  // Wait 1 minute before next test
}

/**
 * @brief Usage Instructions
 * 
 * 1. Update Email Configuration:
 *    - Replace EMAIL_ACCOUNT with your Gmail address
 *    - Replace APP_PASSWORD with your Gmail App Password
 *    - Replace TEST_RECIPIENT with recipient email address
 * 
 * 2. For Gmail App Password:
 *    - Enable 2-Factor Authentication in your Google account
 *    - Generate App Password for "Mail"
 *    - Use App Password instead of regular password
 * 
 * 3. GSM Network Requirements:
 *    - Ensure SIM card is active and has data plan
 *    - Check APN settings with your mobile carrier
 *    - Default APN "internet" works for most carriers
 * 
 * 4. Integration with Main Application:
 *    - This example shows standalone GSM email functionality
 *    - Same methods are integrated in main.cpp
 *    - Use /api/email/gsm/send endpoint for web-based GSM emails
 * 
 * 5. Dual-Mode Email Support:
 *    - WiFi Email: Uses ESP_Mail_Client library (WiFi connection)
 *    - GSM Email: Uses GSM modem built-in SMTP client (GSM connection)
 *    - Both use same SMTP credentials for consistency
 */

