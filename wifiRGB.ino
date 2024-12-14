/**
 * @file Arduino_LED_Communication.ino
 * @brief An Arduino script for Wi-Fi communication to control NeoPixel LED colors remotely.
 * @author Pi Ko (pi.ko@nyu.edu)
 */

#include <Adafruit_NeoPixel.h>
#include <WiFi.h> // Include the WiFi library for connection

// Use GPIO pin 21 as the NeoPixel power pin
#define NEOPIXEL_POWER 21         // Power control pin for NeoPixel
#define NEOPIXEL_PIN PIN_NEOPIXEL // Predefined pin for NeoPixel data

// Wi-Fi credentials
const char* ssid = "Xiaomi_7842";
const char* password = "12345678";

// Create the NeoPixel object
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Debug mode toggle
bool debug_mode = false;

WiFiServer server(80); // Create a server to listen for connections

// Function for debug logging
void DebugLog(const char* message) {
  if (debug_mode) {
    Serial.println(message);
  }
}

void setup() {
  if (debug_mode) {
    Serial.begin(115200);
    while (!Serial) delay(10); // Wait for Serial Monitor if debug mode is on
  }
  DebugLog("Hello, world from QT Py S3!");

  // Enable power to the onboard NeoPixel
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH); // Ensure NeoPixel is powered

  // Initialize the NeoPixel
  pixel.begin();
  pixel.setBrightness(50);  // Adjust brightness (range: 0-255)

  // Connect to Wi-Fi
  DebugLog("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  int attempt_count = 0;
  const int max_attempts = 20; // Maximum number of attempts to connect

  while (WiFi.status() != WL_CONNECTED && attempt_count < max_attempts) {
    pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow (connecting)
    pixel.show();
    DebugLog("Attempting to connect...");
    delay(500);
    attempt_count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    DebugLog("Connected to Wi-Fi!");
    DebugLog(WiFi.localIP().toString().c_str()); // Log the IP address
    pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green (connected)
    server.begin(); // Start the server
    DebugLog("Server started.");
  } else {
    DebugLog("Failed to connect to Wi-Fi.");
    pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red (failed)
  }

  pixel.show();
  delay(1000);
}

void loop() {
  // Check for incoming client connections
  WiFiClient client = server.available();
  if (client) {
    DebugLog("Client connected.");
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == 'R') {
              pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
              pixel.show();
              DebugLog("Set color: Red");
            } else if (c == 'G') {
              pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green
              pixel.show();
              DebugLog("Set color: Green");
            } else if (c == 'B') {
              pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // Blue
              pixel.show();
              DebugLog("Set color: Blue");
            } else {
              DebugLog("Invalid command.");
            }
          pixel.show();
          delay(10); // Add a small delay to ensure color is updated
          DebugLog("Color update sent to NeoPixel.");
          client.flush();
          break;
      }
    }
    delay(10); // Allow time for request processing
    client.stop();
    DebugLog("Client disconnected.");
  }

  if (WiFi.status() != WL_CONNECTED) {
    DebugLog("Wi-Fi disconnected. Attempting to reconnect...");
    pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red (disconnected)
    pixel.show();
    WiFi.begin(ssid, password);

    int attempt_count = 0;
    const int max_attempts = 20;

    while (WiFi.status() != WL_CONNECTED && attempt_count < max_attempts) {
      pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow (reconnecting)
      pixel.show();
      DebugLog("Reconnecting...");
      delay(500);
      attempt_count++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      DebugLog("Reconnected to Wi-Fi!");
      pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green (connected)
    } else {
      DebugLog("Failed to reconnect to Wi-Fi.");
      pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red (failed)
    }

    pixel.show();
    delay(10);
  }

  delay(500); // Delay to prevent excessive loop execution
}

