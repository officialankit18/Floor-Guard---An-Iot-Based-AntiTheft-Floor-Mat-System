# Floor Guard - IoT-Based Anti-Theft Floor Mat System

A comprehensive IoT security system that combines motion detection, camera capture, and automated email alerts to protect areas from unauthorized access.

## Project Structure

```
QBX3/
├── FloorMat_Sensor/
│   └── floormat_sensor.ino              # Floor mat pressure sensor code
│
└── ESP32_CAM_System/
    ├── esp32_cam_main.ino               # Main ESP32-CAM sketch
    ├── config.h                         # WiFi & Email credentials
    ├── camera_config.h                  # Camera GPIO definitions
    ├── camera_functions.h               # Camera initialization & capture
    ├── email_functions.h                # Email sending functions
    ├── wifi_functions.h                 # WiFi connection functions
    └── lcd_functions.h                  # LCD display functions
```

## System Components

### 1. Floor Mat Sensor (`floormat_sensor.ino`)
- Reads analog input from pressure sensor on floor mat
- Triggers camera when pressure > 400 units
- Activates buzzer alert simultaneously
- Operates on Arduino-compatible boards

**Connections:**
- Sensor Input: A0 (Analog)
- Camera Trigger: Pin 7 (OUTPUT)
- Buzzer: Pin 5 (OUTPUT)

### 2. ESP32-CAM Main System
Complete WiFi-enabled camera system with email alerts

**Hardware Requirements:**
- ESP32-CAM (AI-Thinker model)
- 16x2 I2C LCD Display
- Push button (for motion detection)
- Micro USB power supply

**Key Features:**
- WiFi connectivity
- Real-time motion detection
- Automatic photo capture
- Email alerts with photo attachment
- LCD status display
- LittleFS storage

## Installation & Setup

### Prerequisites
1. Arduino IDE with ESP32 board support
2. Required Libraries:
   ```
   - esp32-camera
   - ESP Mail Client by Mobizt
   - LiquidCrystal_I2C by Frank de Brabander
   ```

### Hardware Connections

**I2C LCD Display:**
```
SDA → GPIO 15
SCL → GPIO 14
```

**Push Button:**
```
Button → GPIO 12 (with pull-up resistor)
```

**Camera Pins:**
- PWDN → GPIO 32
- D0-D7 → GPIO 5, 18, 19, 21, 36, 39, 34, 35
- XCLK → GPIO 0
- PCLK → GPIO 22
- VSYNC → GPIO 25
- HREF → GPIO 23
- SIOD → GPIO 26 (I2C SDA)
- SIOC → GPIO 27 (I2C SCL)

### Configuration

1. **Update WiFi Credentials** (`config.h`):
   ```c
   const char* WIFI_SSID = "YOUR_SSID";
   const char* WIFI_PASSWORD = "YOUR_PASSWORD";
   ```

2. **Update Email Credentials** (`config.h`):
   ```c
   #define EMAIL_SENDER_ACCOUNT "your_email@gmail.com"
   #define EMAIL_SENDER_PASSWORD "your_app_password"
   #define EMAIL_RECIPIENT "recipient@gmail.com"
   ```

3. **Upload to ESP32:**
   - Select "AI Thinker ESP32-CAM" board
   - Set Flash Frequency to 80 MHz
   - Upload the sketch

## How It Works

1. **Initialization Phase:**
   - LCD displays boot message
   - WiFi connects to configured network
   - Camera initializes
   - System enters waiting state

2. **Motion Detection Phase:**
   - Button press (GPIO 12) triggers motion detection
   - LCD displays "Motion Detected"
   - Camera captures photo (skips first 5 frames for quality)
   - Photo saved to LittleFS

3. **Alert Phase:**
   - SMTP connection established
   - Email composed with HTML content
   - Photo attached to email
   - Email sent to recipient
   - Status displayed on LCD

4. **Debounce Phase:**
   - System waits 2 seconds
   - Returns to waiting state

## Email Configuration

### For Gmail:
1. Enable "Less secure app access" or use App Passwords
2. Generate App Password from Security settings
3. Use App Password in `config.h`

### SMTP Settings:
- Server: smtp.gmail.com
- Port: 465 (SSL/TLS)

## LCD Display Messages

| State | Line 1 | Line 2 |
|-------|--------|--------|
| Boot | Floor Guard | Initializing... |
| WiFi Connecting | Connecting... | .... |
| WiFi Connected | Connected... | [IP Address] |
| Waiting | Ready - Waiting | for Motion... |
| Motion Detected | Motion Detected | Capturing... |
| Capturing | Capturing Photo | |
| Sending | Sending Email... | |
| Success | Email sent | successfully! |
| Error | Error sending | Email! |

## Troubleshooting

### Camera Not Initializing
- Check GPIO connections
- Verify power supply (5V, ≥500mA)
- Try different XCLK frequency (change in `camera_config.h`)

### WiFi Connection Fails
- Verify SSID and password in `config.h`
- Check 2.4GHz WiFi availability
- Check signal strength

### Email Not Sending
- Verify credentials in `config.h`
- Check Gmail App Password is correct
- Verify recipient email address
- Check internet connection

### LCD Display Issues
- Verify I2C address (0x27)
- Check SDA/SCL pin connections
- Test with I2C Scanner sketch

## Power Consumption

- Idle: ~80-100mA
- WiFi Active: ~150-200mA
- Camera Active: ~200-300mA
- Email Sending: ~300-400mA

Recommended Power Supply: 5V, 2A

## Safety Notes

⚠️ **Security Warning:**
- Do NOT commit credentials to public repositories
- Use environment variables for production
- Enable HTTPS-only Gmail connections
- Regularly update firmware

## Future Enhancements

- [ ] Multiple email recipients
- [ ] Cloud storage integration (AWS S3, Firebase)
- [ ] Mobile app notification
- [ ] Machine learning-based false alarm reduction
- [ ] Battery backup system
- [ ] Multi-camera support
- [ ] Web dashboard for monitoring

## License

This project is part of the Floor Guard IoT Security System.

## Support & Documentation

For more information and detailed API documentation, refer to:
- [ESP32 Documentation](https://docs.espressif.com/)
- [ESP Mail Client Docs](https://github.com/mobizt/ESP-Mail-Client)
- [ESP32-CAM Guide](https://randomnerdtutorials.com/esp32-cam-video-streaming-face-recognition-arduino-ide/)

---

**Version:** 1.0  
**Last Updated:** 2024  
**Author:** Floor Guard Development Team
