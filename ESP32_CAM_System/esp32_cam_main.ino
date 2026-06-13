/**
 * Floor Guard - ESP32-CAM Main Code
 * 
 * IoT-Based Anti-Theft Floor Mat System
 * 
 * Features:
 * - WiFi connectivity
 * - Motion/button triggered photo capture
 * - Email alert with photo attachment
 * - LCD status display
 * - LittleFS photo storage
 * 
 * Required Libraries:
 * - esp32-camera by espressif
 * - ESP Mail Client by Mobizt
 * - LiquidCrystal_I2C by Frank de Brabander
 * 
 * Author: Floor Guard Team
 * Date: 2024
 */

// ========== INCLUDES ==========
#include "esp_camera.h"
#include "SPI.h"
#include "driver/rtc_io.h"
#include "esp_mail_client.h"
#include "FS.h"
#include "SPIFFS.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"

// ========== HEADER FILES ==========
#include "config.h"
#include "camera_config.h"
#include "camera_functions.h"
#include "email_functions.h"
#include "wifi_functions.h"
#include "lcd_functions.h"

// ========== GLOBAL VARIABLES ==========
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
SMTP_Session smtp;

// ========== SETUP FUNCTION ==========
void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== Floor Guard System Starting ===");
  Serial.printf("ESP32 Chip ID: %X\n", ESP.getChipId());
  
  // Initialize LCD Display
  initializeLCD();
  delay(1000);
  
  // Initialize Button Pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("✓ Button pin configured");
  
  // Connect to WiFi
  initializeWiFi();
  delay(2000);
  
  // Initialize SPIFFS Filesystem
  if (!SPIFFS.begin(true)) {
    Serial.println("✗ SPIFFS mount failed!");
    displayStatus("SPIFFS Error", "Mount Failed");
    return;
  }
  Serial.println("✓ SPIFFS initialized successfully!");
  
  // Set email client SSL verification
  MailClient.networkAccessMode(FIREBASE_ACCESS_MODE_WIFI);
  MailClient.errorConnectionCallback(networkConnection);
  
  // Initialize Camera
  if (!initializeCamera()) {
    Serial.println("✗ System halted due to camera initialization failure!");
    return;
  }
  
  // Initialize filesystem for email
  ESP_MAIL_DEFAULT_FLASH_FS.begin();
  
  // Display ready state
  displayWaitingState();
  delay(1000);
  
  Serial.println("\n=== System Ready ===\n");
}

// ========== MAIN LOOP FUNCTION ==========
void loop() {
  // Check if button is pressed (LOW = pressed due to pull-up resistor)
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("\n▶ Motion Detected! Button Pressed...");
    
    // Display motion detection on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected ");
    lcd.setCursor(0, 1);
    lcd.print("Capturing...    ");
    
    // Capture photo
    capturePhotoSaveLittleFS();
    delay(1000);
    
    // Send email alert
    Serial.println("▶ Sending email alert...");
    lcd.setCursor(0, 1);
    lcd.print("Sending Email...");
    sendPhoto();
    
    // Debounce delay to avoid multiple captures
    delay(DEBOUNCE_DELAY);
    
    // Return to waiting state
    displayWaitingState();
    Serial.println("▶ System ready for next detection\n");
  }
}

// ========== NETWORK CONNECTION CALLBACK ==========
void networkConnection() {
  // Callback for network connection status
  Serial.println("Network connection callback");
}
