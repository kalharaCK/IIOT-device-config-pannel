#ifndef DRD_MANAGER_H
#define DRD_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

/**
 * @brief Double Reset Detection Manager
 * 
 * Detects when the ESP32 is reset twice within a configurable time window.
 * Uses NVS (Non-Volatile Storage) via Preferences to persist reset detection
 * state across power cycles.
 * 
 * Usage:
 *   DRD_Manager drd(3000);  // 3 second window
 *   if (drd.detectDoubleReset()) {
 *     // Load secondary dashboard
 *   } else {
 *     // Load primary dashboard
 *   }
 */
class DRD_Manager {
private:
  Preferences prefs;
  const char* NAMESPACE = "drd";
  const char* KEY_FLAG = "flag";
  const char* KEY_TIME = "time";
  const char* KEY_COUNT = "count";
  
  uint32_t timeout;           // Detection window in milliseconds
  bool doubleResetDetected;   // Detection result flag
  
public:
  /**
   * @brief Constructor
   * @param timeoutMs Time window for double reset detection (default: 3000ms)
   */
  DRD_Manager(uint32_t timeoutMs = 3000) : timeout(timeoutMs), doubleResetDetected(false) {}
  
  /**
   * @brief Detect if a double reset occurred
   * @return true if double reset detected, false otherwise
   * 
   * Call this in setup() as early as possible.
   * Detection logic:
   * 1. Check if flag exists and is within timeout window
   * 2. If yes, increment counter and check if >= 2
   * 3. If double reset detected, clear flag and return true
   * 4. If not, set flag with current time
   */
  bool detectDoubleReset() {
    if (!prefs.begin(NAMESPACE, false)) {
      Serial.println("ERROR: Cannot open DRD preferences");
      return false;
    }
    
    bool flagExists = prefs.isKey(KEY_FLAG);
    uint32_t lastResetTime = prefs.getUInt(KEY_TIME, 0);
    uint32_t resetCount = prefs.getUInt(KEY_COUNT, 0);
    uint32_t currentTime = millis();
    
    Serial.println("=== DRD Detection ===");
    Serial.printf("Flag exists: %s\n", flagExists ? "YES" : "NO");
    Serial.printf("Last reset: %u ms ago\n", currentTime - lastResetTime);
    Serial.printf("Reset count: %u\n", resetCount);
    Serial.printf("Timeout window: %u ms\n", timeout);
    
    if (flagExists) {
      // Flag exists - check if within timeout window
      uint32_t timeSinceLastReset = currentTime - lastResetTime;
      
      if (timeSinceLastReset < timeout) {
        // Within timeout window - increment counter
        resetCount++;
        Serial.printf("Within window! Count now: %u\n", resetCount);
        
        if (resetCount >= 2) {
          // Double reset detected!
          Serial.println("DOUBLE RESET DETECTED!");
          doubleResetDetected = true;
          
          // Clear the flag
          prefs.clear();
          prefs.end();
          Serial.println("DRD flag cleared");
          return true;
        } else {
          // First reset in window, update counter
          prefs.putUInt(KEY_COUNT, resetCount);
          prefs.putUInt(KEY_TIME, currentTime);
        }
      } else {
        // Outside timeout window - reset counter
        Serial.println("Outside window - resetting counter");
        prefs.putUInt(KEY_COUNT, 1);
        prefs.putUInt(KEY_TIME, currentTime);
      }
    } else {
      // No flag exists - first reset, create flag
      Serial.println("First reset - creating flag");
      prefs.putBool(KEY_FLAG, true);
      prefs.putUInt(KEY_TIME, currentTime);
      prefs.putUInt(KEY_COUNT, 1);
    }
    
    prefs.end();
    doubleResetDetected = false;
    return false;
  }
  
  /**
   * @brief Clear the double reset detection flag
   * 
   * Call this after the timeout period has elapsed in normal operation
   * to reset the detection state. This prevents false positives from
   * resets that occur after the timeout window.
   */
  void clearFlag() {
    if (prefs.begin(NAMESPACE, false)) {
      prefs.clear();
      prefs.end();
      Serial.println("DRD flag cleared");
    }
  }
  
  /**
   * @brief Check if double reset was detected
   * @return true if double reset was detected during initialization
   */
  bool wasDoubleResetDetected() const {
    return doubleResetDetected;
  }
  
  /**
   * @brief Set a custom timeout window
   * @param timeoutMs New timeout in milliseconds
   */
  void setTimeout(uint32_t timeoutMs) {
    timeout = timeoutMs;
  }
  
  /**
   * @brief Get current timeout setting
   * @return Timeout in milliseconds
   */
  uint32_t getTimeout() const {
    return timeout;
  }
  
  /**
   * @brief Loop function to auto-clear flag after timeout
   * 
   * Call this in loop() to automatically clear the flag after
   * the timeout period has elapsed. This ensures the flag doesn't
   * persist indefinitely.
   */
  void loop() {
    static unsigned long startTime = millis();
    static bool flagCleared = false;
    
    if (!flagCleared && (millis() - startTime) > timeout) {
      if (!doubleResetDetected) {
        clearFlag();
      }
      flagCleared = true;
    }
  }
};

#endif // DRD_MANAGER_H