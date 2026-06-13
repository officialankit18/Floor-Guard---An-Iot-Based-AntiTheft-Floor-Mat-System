/**
 * FLOOR GUARD - Complete IoT Anti-Theft System
 * 
 * Single file with all functionality integrated
 * ESP32-CAM with motion detection and email alerts
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

// ========== CONFIGURATION SECTION ==========

// WiFi Credentials
const char* WIFI_SSID = "CAMERA";
const char* WIFI_PASSWORD = "12341234";

// Email Credentials
#define EMAIL_SENDER_ACCOUNT "heisenbergxmatt@gmail.com"
#define EMAIL_SENDER_PASSWORD "zmmw gqiv bzaq wyid"
#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_SERVER_PORT 465
#define EMAIL_SUBJECT "Movement Detected...!!"
#define EMAIL_RECIPIENT "browncake444@gmail.com"

// Hardware Pins
#define BUTTON_PIN 12
#define LCD_SDA_PIN 15
#define LCD_SCL_PIN 14
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// File Paths
#define FILE_PHOTO_PATH "/photo.jpg"

// Camera Settings
#define INITIAL_FRAME_SKIP 5
#define DEBOUNCE_DELAY 2000

// ========== CAMERA MODEL - AI THINKER ==========
#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM    32
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    0
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      21
  #define Y4_GPIO_NUM      19
  #define Y3_GPIO_NUM      18
  #define Y2_GPIO_NUM      5
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22
#else
  #error "Camera model not selected"
#endif

// ========== GLOBAL VARIABLES ==========
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
SMTP_Session smtp;

// ========== CAMERA CONFIGURATION FUNCTION ==========
void configureCameraSettings(camera_config_t &config) {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
}

// ========== LCD FUNCTIONS ==========
void initializeLCD() {
  Serial.println("Initializing LCD...");
  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Floor Guard");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  Serial.println("✓ LCD initialized successfully!");
  delay(1000);
}

void displayStatus(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayWaitingState() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready - Waiting ");
  lcd.setCursor(0, 1);
  lcd.print("for Motion...   ");
}

// ========== WiFi FUNCTIONS ==========
void initializeWiFi() {
  Serial.println("\n=== WiFi Connection ===");
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...   ");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int connectionAttempts = 0;
  const int maxAttempts = 20;
  
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
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected!      ");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString().c_str());
    delay(2000);
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!    ");
    lcd.setCursor(0, 1);
    lcd.print("Check SSID/PWD  ");
  }
}

// ========== CAMERA FUNCTIONS ==========
bool initializeCamera() {
  camera_config_t config;
  
  configureCameraSettings(config);
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("✗ Camera initialization failed!");
    Serial.printf("Error code: 0x%x\n", err);
    
    lcd.setCursor(0, 0);
    lcd.print("Camera Error    ");
    lcd.setCursor(0, 1);
    lcd.print("Init Failed     ");
    return false;
  }
  
  Serial.println("✓ Camera initialized successfully!");
  return true;
}

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

// ========== EMAIL FUNCTIONS ==========
void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  
  if (status.success()) {
    Serial.println("✓ Email sent successfully!");
    Serial.println("------- Email Sent -------");
    
    lcd.setCursor(0, 0);
    lcd.print("Email sent      ");
    lcd.setCursor(0, 1);
    lcd.print("successfully!   ");
    delay(2000);
    
    smtp.sendingResult.clear();
  }
}

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

// ========== NETWORK CALLBACK ==========
void networkConnection() {
  Serial.println("Network connection callback");
}

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
