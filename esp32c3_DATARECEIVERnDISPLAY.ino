#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Pin I2C ESP32-C3
#define SDA_PIN 8
#define SCL_PIN 9

// Pin UART ESP32-C3
#define RX_PIN 20
#define TX_PIN 21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String incomingData = "";
float distance = 0.0;

void setup() {
  Serial.begin(115200);

  // Inisialisasi UART Hardware 1 khusus pin 20 & 21
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Inisialisasi I2C OLED
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Gagal Inisialisasi!");
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("  SYSTEM INITIALIZED");
  display.setCursor(0, 35);
  display.println(" Waiting UART Data...");
  display.display();
  delay(1500);
}

void loop() {
  // Cek apakah ada data masuk dari Arduino Uno via Serial1
  if (Serial1.available() > 0) {
    incomingData = Serial1.readStringUntil('\n');
    incomingData.trim(); // Hapus karakter whitespace/newline

    distance = incomingData.toFloat();

    // Render data ke Layar OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("-- PENGUKUR JARAK --");

    if (distance < 0 || distance > 400) {
      display.setTextSize(1);
      display.setCursor(10, 30);
      display.println("DI LUAR JANGKAUAN!");
    } else {
      display.setTextSize(2);
      display.setCursor(15, 25);
      display.print(distance, 1);
      display.setTextSize(1);
      display.print(" cm");

      // Visual progress bar sederhana
      int barWidth = map(constrain((int)distance, 2, 100), 2, 100, 0, 128);
      display.drawRect(0, 54, 128, 10, WHITE);
      display.fillRect(0, 54, barWidth, 10, WHITE);
    }

    display.display();
  }
}