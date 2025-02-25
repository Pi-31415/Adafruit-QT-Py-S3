/**
 * @file Arduino_Wireless_Wristband_Extended.ino
 * @brief An Arduino script for Wi-Fi communication to control NeoPixel LED colors and motor pulses remotely.
 * 
 * This extended version embeds additional motion functions (up, right, left, down pulses, 
 * as well as clockwise and counterclockwise sequences) directly into the code. 
 * The device connects to Wi-Fi and starts a server. When a numeric command is received, 
 * the corresponding motion function is executed.
 *
 * Command mapping:
 *   '1' : Up Pulse (activates Motor 1)
 *   '2' : Right Pulse (activates Motor 2)
 *   '3' : Left Pulse (activates Motor 3)
 *   '4' : Down Pulse (activates Motor 4)
 *   '5' : Clockwise sequence (Motor 1 → Motor 2 → Motor 4 → Motor 3)
 *   '6' : Counterclockwise sequence (Motor 1 → Motor 3 → Motor 4 → Motor 2)
 *   '7' : All Motors Pulse (activates all 4 motors simultaneously)
 *   Other: Stop all motors and log invalid command.
 *
 * @author
 *   Pi Ko (pi.ko@nyu.edu)
 */

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

// Pin definitions
#define NEOPIXEL_POWER 21         // Power control pin for NeoPixel
#define NEOPIXEL_PIN PIN_NEOPIXEL // Predefined pin for NeoPixel data

// Motor control pins
#define MOTOR1_PIN A0
#define MOTOR2_PIN A1
#define MOTOR3_PIN A2
#define MOTOR4_PIN A3

// Wi-Fi credentials
const char* ssid = "Xiaomi_7842";
const char* password = "12345678";

// NeoPixel configuration
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Debug mode toggle (set to true to enable serial debug logging)
bool debug_mode = false;

// Wi-Fi server instance
WiFiServer server(80);

// Timing constants (in milliseconds)
const unsigned long DURATION_ON  = 200;  // Duration motors remain active
const unsigned long DURATION_OFF = 80;   // Duration motors remain off between pulses

/**
 * @brief Outputs a debug message to the serial console if debug mode is enabled.
 * 
 * @param message The debug message to print.
 */
void DebugLog(const char* message) {
  if (debug_mode) {
    Serial.println(message);
  }
}

/**
 * @brief Turns off all motor outputs.
 */
void stopAllMotors() {
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  // Set NeoPixel to white to indicate stop state
  pixel.setPixelColor(0, pixel.Color(255, 255, 255));
  pixel.show();
  DebugLog("All motors stopped.");
}

/**
 * @brief Executes an Up Pulse by activating Motor 1.
 *
 * The function turns on Motor 1 (red indication) for a fixed duration,
 * then turns it off and waits before finishing.
 */
void upPulse() {
  DebugLog("Starting Up Pulse (Motor 1)...");
  // Activate Motor 1 only and set NeoPixel color to red
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
  pixel.show();
  delay(DURATION_ON);
  
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes a Right Pulse by activating Motor 2.
 *
 * The function turns on Motor 2 (green indication) for a fixed duration,
 * then turns it off and waits before finishing.
 */
void rightPulse() {
  DebugLog("Starting Right Pulse (Motor 2)...");
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, HIGH);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green
  pixel.show();
  delay(DURATION_ON);
  
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes a Left Pulse by activating Motor 3.
 *
 * The function turns on Motor 3 (blue indication) for a fixed duration,
 * then turns it off and waits before finishing.
 */
void leftPulse() {
  DebugLog("Starting Left Pulse (Motor 3)...");
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, HIGH);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // Blue
  pixel.show();
  delay(DURATION_ON);
  
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes a Down Pulse by activating Motor 4.
 *
 * The function turns on Motor 4 (yellow indication) for a fixed duration,
 * then turns it off and waits before finishing.
 */
void downPulse() {
  DebugLog("Starting Down Pulse (Motor 4)...");
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, HIGH);
  pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow
  pixel.show();
  delay(DURATION_ON);
  
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes a Clockwise motor sequence.
 *
 * The function sequentially activates motors in the following order:
 * Motor 1 (Red) → Motor 2 (Green) → Motor 4 (Yellow) → Motor 3 (Blue),
 * with each motor active for the on-duration followed by an off-duration.
 */
void clockwisePulse() {
  DebugLog("Starting Clockwise Sequence...");
  // Motor 1
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 2
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, HIGH);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 4
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, HIGH);
  pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 3
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, HIGH);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // Blue
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes a Counterclockwise motor sequence.
 *
 * The function sequentially activates motors in the following order:
 * Motor 1 (Red) → Motor 3 (Blue) → Motor 4 (Yellow) → Motor 2 (Green),
 * with each motor active for the on-duration followed by an off-duration.
 */
void counterClockwisePulse() {
  DebugLog("Starting Counterclockwise Sequence...");
  // Motor 1
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 3
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, HIGH);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // Blue
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 4
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, LOW);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, HIGH);
  pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);

  // Motor 2
  digitalWrite(MOTOR1_PIN, LOW);
  digitalWrite(MOTOR2_PIN, HIGH);
  digitalWrite(MOTOR3_PIN, LOW);
  digitalWrite(MOTOR4_PIN, LOW);
  pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green
  pixel.show();
  delay(DURATION_ON);
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Executes an All Motors Pulse by activating all four motors simultaneously.
 *
 * The function turns on all motors (with a cyan indication) for a fixed duration,
 * then turns them off and waits before finishing.
 */
void allMotorsPulse() {
  DebugLog("Starting All Motors Pulse (all motors)...");
  digitalWrite(MOTOR1_PIN, HIGH);
  digitalWrite(MOTOR2_PIN, HIGH);
  digitalWrite(MOTOR3_PIN, HIGH);
  digitalWrite(MOTOR4_PIN, HIGH);
  pixel.setPixelColor(0, pixel.Color(0, 255, 255)); // Cyan
  pixel.show();
  delay(DURATION_ON);
  
  stopAllMotors();
  delay(DURATION_OFF);
}

/**
 * @brief Initializes hardware and connects to the Wi-Fi network.
 * 
 * The setup function initializes serial communication (if debug is enabled),
 * configures the NeoPixel and motor control pins, and attempts to connect to Wi-Fi.
 */
void setup() {
  // Initialize serial communication if debug mode is on
  if (debug_mode) {
    Serial.begin(115200);
    while (!Serial) delay(10);
  }
  DebugLog("Hello, world from QT Py S3!");

  // Enable power for the NeoPixel
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);

  // Initialize the NeoPixel
  pixel.begin();
  pixel.setBrightness(50);  // Brightness range: 0-255

  // Initialize motor pins as outputs
  DebugLog("Initiating Motors");
  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);
  pinMode(MOTOR3_PIN, OUTPUT);
  pinMode(MOTOR4_PIN, OUTPUT);
  stopAllMotors();

  // Connect to Wi-Fi
  DebugLog("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  int attempt_count = 0;
  const int max_attempts = 20;
  while (WiFi.status() != WL_CONNECTED && attempt_count < max_attempts) {
    pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow indicates connecting
    pixel.show();
    DebugLog("Attempting to connect...");
    delay(500);
    attempt_count++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    DebugLog("Connected to Wi-Fi!");
    DebugLog(WiFi.localIP().toString().c_str());
    pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green indicates connected
    server.begin();
    DebugLog("Server started.");
  } else {
    DebugLog("Failed to connect to Wi-Fi.");
    pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red indicates failure
  }
  pixel.show();
  delay(1000);
}

/**
 * @brief Main loop: processes incoming client commands.
 * 
 * The loop continuously checks for incoming connections on the Wi-Fi server.
 * When a command character is received, it maps the character to a motion function.
 * If the command is not recognized, it stops all motors.
 */
void loop() {
  // Check for incoming client connections
  WiFiClient client = server.available();
  if (client) {
    DebugLog("Client connected.");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        DebugLog("Received command:");
        // Map received command to motion functions
        switch(c) {
          case '1':
            upPulse();
            break;
          case '2':
            rightPulse();
            break;
          case '3':
            leftPulse();
            break;
          case '4':
            downPulse();
            break;
          case '5':
            clockwisePulse();
            break;
          case '6':
            counterClockwisePulse();
            break;
          case '7':
            allMotorsPulse();
            break;
          default:
            DebugLog("Invalid command received. Stopping all motors.");
            stopAllMotors();
            break;
        }
        // Ensure any pending data is flushed and then disconnect client
        client.flush();
        break;
      }
    }
    delay(10);
    client.stop();
    DebugLog("Client disconnected.");
  }

  // Check Wi-Fi connection status and attempt reconnection if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    DebugLog("Wi-Fi disconnected. Attempting to reconnect...");
    pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red indicates disconnection
    pixel.show();
    WiFi.begin(ssid, password);
    int attempt_count = 0;
    const int max_attempts = 20;
    while (WiFi.status() != WL_CONNECTED && attempt_count < max_attempts) {
      pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow indicates reconnecting
      pixel.show();
      DebugLog("Reconnecting...");
      delay(500);
      attempt_count++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      DebugLog("Reconnected to Wi-Fi!");
      pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green indicates connected
    } else {
      DebugLog("Failed to reconnect to Wi-Fi.");
      pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red indicates failure
    }
    pixel.show();
    delay(10);
  }
  delay(5); // Prevent excessive loop iterations
}
