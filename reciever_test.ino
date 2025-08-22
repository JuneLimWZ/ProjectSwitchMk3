#include <esp_now.h>
#include <WiFi.h>

// --- UART connection to Arduino Pro Micro ---
#define PRO_MICRO_TX_PIN 4  // GPIO21 (TXD)
#define PRO_MICRO_RX_PIN 3  // GPIO20 (RXD)

// Structure to hold controller data
struct __attribute__((packed)) ControlData {
  uint16_t joyX;
  uint16_t joyY;
  uint8_t joyPressed;
  uint8_t btn1;
  uint8_t btn2;
};


ControlData receivedData;

// Serial for debugging
HardwareSerial proSerial(1);  // Use UART1 for Pro Micro

// Callback function to handle incoming ESP-NOW data
void onDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (len == sizeof(ControlData)) {
    memcpy(&receivedData, data, sizeof(ControlData));

    // Debug print to Serial Monitor
    Serial.print("Joystick: X=");
    Serial.print(receivedData.joyX);
    Serial.print(" Y=");
    Serial.print(receivedData.joyY);
    Serial.print(" Pressed=");
    Serial.print(receivedData.joyPressed);
    Serial.print(" | BTN1=");
    Serial.print(receivedData.btn1);
    Serial.print(" BTN2=");
    Serial.println(receivedData.btn2);

    // Send data to Pro Micro via UART (start byte, data, end byte)
    proSerial.write(0xAA);  // Start byte
    proSerial.write((uint8_t*)&receivedData, sizeof(ControlData));  // Send the packed data
    proSerial.write(0x55);  // End byte
  } else {
    Serial.println("Received data with incorrect length!");
  }
}

void setup() {
  // Initialize the serial port for debugging
  Serial.begin(115200);

  // Initialize UART1 for Pro Micro communication
  proSerial.begin(4800, SERIAL_8N1, PRO_MICRO_RX_PIN, PRO_MICRO_TX_PIN);  // 4800 baud

  // Set ESP32-C3 to Wi-Fi station mode (necessary for ESP-NOW)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from any networks

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    while (true);  // Halt if initialization fails
  }

  // Register callback function for receiving ESP-NOW data
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP32-C3 is ready to receive controller data");
}

void loop() {
  // Nothing required in loop; all action happens in onDataRecv callback
}
