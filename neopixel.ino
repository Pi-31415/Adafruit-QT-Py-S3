#include <Adafruit_NeoPixel.h>

// Use GPIO pin 21 as the NeoPixel power pin
#define NEOPIXEL_POWER 21         // Power control pin for NeoPixel
#define NEOPIXEL_PIN PIN_NEOPIXEL // Predefined pin for NeoPixel data

// Create the NeoPixel object
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Start the serial connection
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial Monitor to initialize
  Serial.println("Hello, world from QT Py S3!");

  // Enable power to the onboard NeoPixel
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH); // Ensure NeoPixel is powered

  // Initialize the NeoPixel
  pixel.begin();
  pixel.setBrightness(50);  // Adjust brightness (range: 0-255)
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Set to red
  pixel.show();
  delay(1000);
}

void loop() {
  // Cycle the onboard NeoPixel through colors
  pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // Green
  pixel.show();
  Serial.println("NeoPixel: Green");
  delay(1000);

  pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // Blue
  pixel.show();
  Serial.println("NeoPixel: Blue");
  delay(1000);

  pixel.setPixelColor(0, pixel.Color(255, 255, 0)); // Yellow
  pixel.show();
  Serial.println("NeoPixel: Yellow");
  delay(1000);
}
