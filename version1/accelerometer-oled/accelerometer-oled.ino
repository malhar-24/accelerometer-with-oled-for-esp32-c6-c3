#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MPU6050 address
const int MPU_addr = 0x68;

// Raw accelerometer values
int16_t AcX, AcY;

// Display circle parameters
const int centerX = 64;
const int centerY = 32;
const int radius = 25;

// Scaling factor to adjust dot sensitivity
const float scale = 0.002;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Wake up MPU6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed"));
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.print("zintechideas");
  display.display();
  delay(2000);
}

void loop() {
  // === Read accelerometer data ===
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();

  // === Map acceleration to dot position ===
  int dotX = centerX + constrain(AcX * scale, -radius, radius);
  int dotY = centerY - constrain(AcY * scale, -radius, radius);  // Y inverted

  // === Display ===
  display.clearDisplay();

  // Draw boundary circle
  display.drawCircle(centerX, centerY, radius, WHITE);

  // Draw motion dot
  display.fillCircle(dotX, dotY, 2, WHITE);

  // Optional direction labels
  display.setTextSize(1);
  display.setCursor(centerX - 3, centerY - radius - 10); display.print("F");
  display.setCursor(centerX - 3, centerY + radius + 2);  display.print("B");
  display.setCursor(centerX - radius - 10, centerY - 3); display.print("L");
  display.setCursor(centerX + radius + 2, centerY - 3);  display.print("R");

  display.display();

  delay(50);  // Smooth motion
}
