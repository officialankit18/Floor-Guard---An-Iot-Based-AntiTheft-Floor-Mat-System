/**
 * Camera Functions Header
 * 
 * Contains all camera-related functions for photo capture
 * and storage operations.
 */

#ifndef CAMERA_FUNCTIONS_H
#define CAMERA_FUNCTIONS_H

#include "config.h"

extern LiquidCrystal_I2C lcd;

/**
 * Initialize Camera
 * Sets up camera configuration and initializes the camera module
 * 
 * @return true if initialization successful, false otherwise
 */
bool initializeCamera() {
  camera_config_t config;
  
  // Configure camera settings
  configureCameraSettings(config);
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("✗ Camera initialization failed!");
    Serial.printf("Error code: 0x%x\n", err);
    
    // Display error on LCD
    lcd.setCursor(0, 0);
    lcd.print("Camera Error    ");
    lcd.setCursor(0, 1);
    lcd.print("Init Failed     ");
    return false;
  }
  
  Serial.println("✓ Camera initialized successfully!");
  return true;
}

/**
 * Capture Photo and Save to LittleFS
 * Takes a photo from camera and saves it to internal storage
 */
void capturePhotoSaveLittleFS() {
  camera_fb_t* fb = NULL;
  
  Serial.println("Capturing photo...");
  lcd.setCursor(0, 0);
  lcd.print("Capturing Photo ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  
  // Skip first few frames for better quality
  for (int i = 0; i < INITIAL_FRAME_SKIP; i++) {
    fb = esp_camera_fb_get();
    if (fb) {
      esp_camera_fb_return(fb);
    }
  }
  
  // Capture the actual frame
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("✗ Photo capture failed!");
    lcd.setCursor(0, 0);
    lcd.print("Photo Capture   ");
    lcd.setCursor(0, 1);
    lcd.print("Failed!         ");
    return;
  }
  
  // Save to file
  File file = SPIFFS.open(FILE_PHOTO_PATH, FILE_WRITE);
  if (!file) {
    Serial.println("✗ Failed to open file for writing!");
    esp_camera_fb_return(fb);
    return;
  }
  
  // Write photo data to file
  if (file.write(fb->buf, fb->len)) {
    Serial.println("✓ Photo saved successfully!");
    Serial.printf("Photo size: %d bytes\n", fb->len);
    
    // Display success on LCD
    lcd.setCursor(0, 0);
    lcd.print("Photo Captured  ");
    lcd.setCursor(0, 1);
    lcd.print("Successfully    ");
  } else {
    Serial.println("✗ Failed to write photo data!");
  }
  
  file.close();
  esp_camera_fb_return(fb);
}

#endif
