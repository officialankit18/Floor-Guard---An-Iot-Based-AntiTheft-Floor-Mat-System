/**
 * WiFi Functions Header
 * 
 * Contains WiFi initialization and connection management
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include "config.h"

extern LiquidCrystal_I2C lcd;

/**
 * Initialize WiFi Connection
 * Connects to configured WiFi network and displays connection status
 */
void initializeWiFi() {
  Serial.println("\n=== WiFi Connection ===");
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  // Display connection attempt on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...   ");
  
  // Begin WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int connectionAttempts = 0;
  const int maxAttempts = 20;  // 10 seconds timeout
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(connectionAttempts % 16, 1);
    lcd.print(".");
    connectionAttempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Display connection success on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected!      ");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString().c_str());
    delay(2000);
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
    
    // Display connection failure on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!    ");
    lcd.setCursor(0, 1);
    lcd.print("Check SSID/PWD  ");
  }
}

#endif
