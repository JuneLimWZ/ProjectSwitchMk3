#include <esp_now.h>
#include <WiFi.h>

// MAC Address of ESP32-C3 receiver
uint8_t receiverMac[] = { 0x18, 0x8B, 0x0E, 0x1B, 0xB6, 0x5C };

typedef struct __attribute__((packed)) {
  uint16_t joyX;
  uint16_t joyY;
  uint8_t joyPressed;
  uint8_t btn1;
  uint8_t btn2;
} ControlData;


ControlData data;

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(5, INPUT_PULLUP); // Joystick button
  pinMode(2, INPUT_PULLUP); // Button 1
  pinMode(3, INPUT_PULLUP); // Button 2

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW initialized.");
}

void loop() {
  data.joyX = analogRead(0);     // Reading analog values for joystick X axis
  data.joyY = analogRead(1);     // Reading analog values for joystick Y axis
  data.joyPressed = digitalRead(5) == LOW;  // Joystick press detection
  data.btn1 = digitalRead(2) == LOW;        // Button 1 press detection
  data.btn2 = digitalRead(3) == LOW;        // Button 2 press detection

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Send error");
  }

  delay(200);  // Short delay to prevent too rapid updates
}
