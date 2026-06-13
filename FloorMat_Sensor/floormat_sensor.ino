/**
 * Floor Guard - Floor Mat Sensor Code
 * 
 * This code handles the floor mat sensor input and controls
 * the camera trigger and buzzer alert system.
 * 
 * Connections:
 * - Sensor Input: A0 (Analog)
 * - Camera Trigger: Pin 7 (OUTPUT)
 * - Buzzer: Pin 5 (OUTPUT)
 */

void setup() {
  pinMode(7, OUTPUT);        // Camera trigger pin
  pinMode(5, OUTPUT);        // Buzzer pin
  Serial.begin(115200);      // Serial communication
  digitalWrite(5, LOW);      // Initialize buzzer as OFF
}

void loop() {
  int sensorValue = analogRead(A0);  // Read sensor value from floor mat
  
  Serial.println(sensorValue);
  
  if (sensorValue > 400) {
    // Motion/Pressure detected on floor mat
    digitalWrite(7, LOW);      // Trigger camera
    digitalWrite(5, HIGH);     // Activate buzzer
    Serial.println("MOTION DETECTED - Camera Triggered & Buzzer ON");
    delay(1000);
  } else {
    // No motion/pressure detected
    digitalWrite(7, HIGH);     // Deactivate camera trigger
    digitalWrite(5, LOW);      // Deactivate buzzer
    Serial.println("NO MOTION - Camera & Buzzer OFF");
  }
  
  delay(500);  // Check sensor every 500ms
}
