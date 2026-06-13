/**
 * LCD Functions Header
 * 
 * Contains LCD initialization and display management functions
 */

#ifndef LCD_FUNCTIONS_H
#define LCD_FUNCTIONS_H

#include "config.h"

extern LiquidCrystal_I2C lcd;

/**
 * Initialize LCD Display
 * Sets up I2C communication and initializes the 16x2 LCD
 */
void initializeLCD() {
  Serial.println("Initializing LCD...");
  
  // Initialize I2C communication
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Display boot message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Floor Guard");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  Serial.println("✓ LCD initialized successfully!");
  delay(1000);
}

/**
 * Display Status Message
 * Shows a two-line status message on LCD
 */
void displayStatus(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

/**
 * Display Waiting State
 * Shows that the system is ready and waiting for motion
 */
void displayWaitingState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready - Waiting ");
  lcd.setCursor(0, 1);
  lcd.print("for Motion...   ");
}

#endif
