/**
 * Email Functions Header
 * 
 * Contains all email-related functions for SMTP configuration,
 * message composition, and attachment handling.
 */

#ifndef EMAIL_FUNCTIONS_H
#define EMAIL_FUNCTIONS_H

#include "config.h"

extern SMTP_Session smtp;

/**
 * SMTP Callback Function
 * Handles email sending status and results
 */
void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  
  if (status.success()) {
    Serial.println("✓ Email sent successfully!");
    Serial.println("------- Email Sent -------");
    
    // Update LCD
    lcd.setCursor(0, 0);
    lcd.print("Email sent      ");
    lcd.setCursor(0, 1);
    lcd.print("successfully!   ");
    delay(2000);
    
    // Clear sending results
    smtp.sendingResult.clear();
  }
}

/**
 * Send Email with Photo Attachment
 * Configures SMTP session and sends alert email with captured photo
 */
void sendPhoto() {
  smtp.debug(1);
  smtp.callback(smtpCallback);
  
  // Session Configuration
  Session_Config config;
  config.server.host_name = SMTP_SERVER;
  config.server.port = SMTP_SERVER_PORT;
  config.login.email = EMAIL_SENDER_ACCOUNT;
  config.login.password = EMAIL_SENDER_PASSWORD;
  
  // Email Message Setup
  SMTP_Message message;
  message.sender.name = "ESP32-CAM";
  message.sender.email = EMAIL_SENDER_ACCOUNT;
  message.subject = EMAIL_SUBJECT;
  message.addRecipient(EMAIL_RECIPIENT, EMAIL_RECIPIENT);
  
  // Email Body with HTML
  String htmlMsg = "<h2>Floor Guard Alert</h2>";
  htmlMsg += "<p><b>ESP32-CAM Motion Detection System</b></p>";
  htmlMsg += "<p>Movement detected! Captured photo attached.</p>";
  htmlMsg += "<p>Timestamp: ";
  htmlMsg += __DATE__;
  htmlMsg += " ";
  htmlMsg += __TIME__;
  htmlMsg += "</p>";
  
  message.html.content = htmlMsg.c_str();
  message.html.charSet = "utf-8";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_qp;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
  
  // Attachment - Photo
  SMTP_Attachment att;
  att.descr.filename = "motion_detection.jpg";
  att.descr.mime = "image/jpeg";
  att.file.path = FILE_PHOTO_PATH;
  att.file.storage_type = esp_mail_file_storage_type_flash;
  att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;
  message.addAttachment(att);
  
  // Connect to SMTP Server
  if (!smtp.connect(&config)) {
    return;
  }
  
  // Send Email
  if (!MailClient.sendMail(&smtp, &message, true)) {
    Serial.println("✗ Error sending Email: " + smtp.errorReason());
    
    lcd.setCursor(0, 0);
    lcd.print("Error sending   ");
    lcd.setCursor(0, 1);
    lcd.print("Email!          ");
  }
}

#endif
