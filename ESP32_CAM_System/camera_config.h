/**
 * Camera Configuration Header
 * 
 * Contains camera model selection and GPIO pin definitions
 * for ESP32-CAM module (AI-Thinker model)
 */

#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

// ========== Camera Model Selection ==========
#define CAMERA_MODEL_AI_THINKER

// ========== AI-Thinker ESP32-CAM GPIO Pin Definitions ==========
#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM    32   // Power Down
  #define RESET_GPIO_NUM   -1   // Reset (not used)
  #define XCLK_GPIO_NUM    0    // Master Clock
  #define SIOD_GPIO_NUM    26   // SCCB Data
  #define SIOC_GPIO_NUM    27   // SCCB Clock
  
  #define Y9_GPIO_NUM      35   // Data pin 9
  #define Y8_GPIO_NUM      34   // Data pin 8
  #define Y7_GPIO_NUM      39   // Data pin 7
  #define Y6_GPIO_NUM      36   // Data pin 6
  #define Y5_GPIO_NUM      21   // Data pin 5
  #define Y4_GPIO_NUM      19   // Data pin 4
  #define Y3_GPIO_NUM      18   // Data pin 3
  #define Y2_GPIO_NUM      5    // Data pin 2
  #define VSYNC_GPIO_NUM   25   // Vertical Sync
  #define HREF_GPIO_NUM    23   // Horizontal Reference
  #define PCLK_GPIO_NUM    22   // Pixel Clock
  
#else
  #error "Camera model not selected. Define CAMERA_MODEL_AI_THINKER or other supported model."
#endif

// ========== Camera Configuration Structure ==========
void configureCameraSettings(camera_config_t &config) {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  
  // Data pins
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  
  // Control pins
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  // Clock settings
  config.xclk_freq_hz = 20000000;  // 20MHz
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  
  // Resolution and quality settings based on available PSRAM
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;  // Ultra HD
    config.jpeg_quality = 10;             // High quality
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;  // Standard HD
    config.jpeg_quality = 12;             // Medium quality
    config.fb_count = 1;
  }
}

#endif
