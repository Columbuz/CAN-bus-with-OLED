#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_CAN.h>

// OLED Display Configurations
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// CAN Variables
uint32_t recv_id = 0;
uint8_t recv_length = 0;
uint32_t msg_count = 0;  // Counter for received CAN messages

void setup() {
  // Start Serial Communication
  Serial.begin(115200);
  while (!Serial) {}

  // Initialize CAN Communication
  if (!CAN.begin(CanBitRate::BR_500k)) {
    Serial.println("CAN.begin(...) failed.");
    while (true); // Stop execution if CAN fails
  }
  Serial.println("CAN communication started");

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED initialization failed.");
    while (true); // Stop execution if OLED fails
  }

  // Clear and Display Initial OLED Screen
  display.clearDisplay();
  oled("CAN ID: N/A", "Length: N/A", "Msg Count: 0");
}

void loop() {
  if (CAN.available()) {
    // Read CAN Message
    CanMsg const msg = CAN.read();
    recv_id = msg.id;  // Use `msg.getExtendedId()` if extended ID is used
    recv_length = msg.data_length;
    msg_count++;  // Increment message count

    // Print CAN Details to Serial
    Serial.println("CAN Msg Successful"); 
    Serial.print("CAN ID: ");
    Serial.println(recv_id, HEX);
    Serial.print("Length: ");
    Serial.println(recv_length);

    // Print Message Data
    Serial.print("Data: ");
    for (uint8_t i = 0; i < recv_length; i++) {
      Serial.print("0x");
      Serial.print(msg.data[i], HEX);  // Print each byte in HEX format
      if (i < recv_length - 1) {
        Serial.print(", ");
      }
    }
    Serial.println();

    Serial.print("Messages Received: ");
    Serial.println(msg_count);

    // Update OLED with CAN Data
    oled("CAN ID: " + String(recv_id, HEX),
         "Length: " + String(recv_length),
         "Msg Count: " + String(msg_count) + " OK!");
  } else {
    // No CAN message received
    Serial.println("No CAN message received");

    // Update OLED to indicate no messages received
    oled("Sorry :(", "No CAN message", "Msg Count: " + String(msg_count));
  }

  delay(500); // Wait for 500ms
}

void oled(String line1, String line2, String line3) {
  // Update OLED Display with CAN Data or error message
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Line 1
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println(line1);

  // Line 2
  display.setCursor(0, 30);
  display.println(line2);

  // Line 3
  display.setCursor(0, 50);
  display.println(line3);

  display.display();
}
