#include <WiFi.h>

// =====================
// Nintendo Switch masks
// =====================
#define DPAD_UP_MASK_ON       0x00
#define DPAD_UPRIGHT_MASK_ON  0x01
#define DPAD_RIGHT_MASK_ON    0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON     0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON     0x06
#define DPAD_UPLEFT_MASK_ON   0x07
#define DPAD_NOTHING_MASK_ON  0x08

#define A_MASK_ON    0x04
#define B_MASK_ON    0x02
#define X_MASK_ON    0x08
#define Y_MASK_ON    0x01
#define ZL_MASK_ON   0x40
#define ZR_MASK_ON   0x80
#define START_MASK_ON  0x200
#define HOME_MASK_ON   0x1000

// =====================
// HID Report structure
// =====================
typedef struct __attribute__((packed)) {
  uint16_t Button;  // Bitmask of buttons
  uint8_t HAT;      // D-pad (HAT switch)
  uint8_t LX;       // Left joystick X (0-255)
  uint8_t LY;       // Left joystick Y (0-255)
  uint8_t RX;       // Right joystick X
  uint8_t RY;       // Right joystick Y
} ReportData_t;

ReportData_t ReportData;

// =====================
// Pin Mapping
// =====================
#define PIN_JOY_X 0
#define PIN_JOY_Y 1
#define PIN_JOY_BTN 5
#define PIN_BTN1 2   // mapped to A
#define PIN_BTN2 3   // mapped to B

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);

  Serial.println("Controller test started.");
}

// =====================
// Helpers
// =====================
void processDPAD(int xAxis, int yAxis) {
  if ((yAxis < 200) && (xAxis > 800)) ReportData.HAT = DPAD_UPRIGHT_MASK_ON;
  else if ((yAxis > 800) && (xAxis > 800)) ReportData.HAT = DPAD_DOWNRIGHT_MASK_ON;
  else if ((yAxis > 800) && (xAxis < 200)) ReportData.HAT = DPAD_DOWNLEFT_MASK_ON;
  else if ((yAxis < 200) && (xAxis < 200)) ReportData.HAT = DPAD_UPLEFT_MASK_ON;
  else if (yAxis < 200) ReportData.HAT = DPAD_UP_MASK_ON;
  else if (yAxis > 800) ReportData.HAT = DPAD_DOWN_MASK_ON;
  else if (xAxis < 200) ReportData.HAT = DPAD_LEFT_MASK_ON;
  else if (xAxis > 800) ReportData.HAT = DPAD_RIGHT_MASK_ON;
  else ReportData.HAT = DPAD_NOTHING_MASK_ON;
}

void buttonProcessing(bool btnA, bool btnB, bool joyPressed) {
  ReportData.Button = 0; // reset
  if (btnA) ReportData.Button |= A_MASK_ON;
  if (btnB) ReportData.Button |= B_MASK_ON;
  if (joyPressed) ReportData.Button |= START_MASK_ON;  // test: joystick press → Start
}

// =====================
// Main Loop
// =====================
void loop() {
  int rawX = analogRead(PIN_JOY_X);
  int rawY = analogRead(PIN_JOY_Y);

  // Invert Y (up = 0, down = 1023 like your wired code)
  rawY = 1023 - rawY;

  // Map to 0–255
  ReportData.LX = map(rawX, 0, 1023, 0, 255);
  ReportData.LY = map(rawY, 0, 1023, 0, 255);
  ReportData.RX = 128; // centered
  ReportData.RY = 128; // centered

  // Process HAT (D-Pad)
  processDPAD(rawX, rawY);

  // Process Buttons
  bool joyPressed = (digitalRead(PIN_JOY_BTN) == LOW);
  bool btnA = (digitalRead(PIN_BTN1) == LOW);
  bool btnB = (digitalRead(PIN_BTN2) == LOW);
  buttonProcessing(btnA, btnB, joyPressed);

  // Print results
  Serial.print("Buttons: ");
  Serial.print(ReportData.Button, BIN);
  Serial.print(" | HAT: ");
  Serial.print(ReportData.HAT);
  Serial.print(" | LX: ");
  Serial.print(ReportData.LX);
  Serial.print(" | LY: ");
  Serial.print(ReportData.LY);
  Serial.print(" | RX: ");
  Serial.print(ReportData.RX);
  Serial.print(" | RY: ");
  Serial.println(ReportData.RY);

  delay(200); // ~5Hz printing so it's readable
}
