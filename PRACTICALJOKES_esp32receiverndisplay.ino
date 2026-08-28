#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#define SDA_PIN 8
#define SCL_PIN 9
#define RX_PIN 20
#define TX_PIN 21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String incomingData = "";
float distance = 0.0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  if (Serial1.available() > 0) {
    incomingData = Serial1.readStringUntil('\n');
    incomingData.trim();
    distance = incomingData.toFloat();

    display.clearDisplay();

    // Tampilan Khusus saat jarak <= 5cm
    if (distance > 0 && distance <= 5.0) {
      display.setTextSize(2);
      display.setCursor(5, 10);
      display.println("BAHAYA!!!");
      display.setTextSize(1);
      display.setCursor(0, 38);
      display.println("SAYA AKAN LAWAN!");
      display.drawRect(0, 0, 128, 64, WHITE); // Frame peringatan
    } 
    else if (distance < 0 || distance > 400) {
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("-- PENGUKUR JARAK --");
      display.setCursor(10, 30);
      display.println("DI LUAR JANGKAUAN!");
    } 
    else {
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("-- PENGUKUR JARAK --");
      display.setTextSize(2);
      display.setCursor(15, 25);
      display.print(distance, 1);
      display.setTextSize(1);
      display.print(" cm");

      int barWidth = map(constrain((int)distance, 2, 100), 2, 100, 0, 128);
      display.drawRect(0, 54, 128, 10, WHITE);
      display.fillRect(0, 54, barWidth, 10, WHITE);
    }

    display.display();
  }
}