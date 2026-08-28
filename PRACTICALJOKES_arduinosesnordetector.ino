#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define TRIG_PIN 2
#define ECHO_PIN 3

// serialESP HANYA untuk mengirim data (RX = -1 / tidak digunakan, TX = Pin 10)
SoftwareSerial serialESP(-1, 10);

// serialDFP untuk komunikasi DFPlayer (RX = Pin 4, TX = Pin 5)
SoftwareSerial serialDFP(4, 5);

DFRobotDFPlayerMini myDFPlayer;

long duration;
float distanceCm;
bool isSoundTriggered = false; // Flag pelindung agar audio tidak diputar berulang

void setup() {
  Serial.begin(9600); // Serial Monitor untuk Debugging USB

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  serialESP.begin(9600);
  serialDFP.begin(9600);

  Serial.println(F("Inisialisasi DFPlayer..."));

  // Parameter isACK diset 'false' agar non-blocking (tidak menunggu balasan yang bikin macet)
  if (!myDFPlayer.begin(serialDFP, /*isACK =*/ false, /*doReset =*/ true)) {
    Serial.println(F("DFPlayer Tidak Merespons! Periksa MicroSD/Wiring."));
  } else {
    Serial.println(F("DFPlayer Siap!"));
    myDFPlayer.volume(28); // Volume (0 - 30)
  }
}

void loop() {
  // 1. Baca Jarak dari Sensor HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms (~5 meter)
  distanceCm = (duration * 0.0343) / 2.0;

  // 2. Kirim Data Jarak ke ESP32-C3 (Jalur Seri Non-blocking)
  if (duration > 0 && distanceCm <= 400) {
    serialESP.println(distanceCm, 1);
  } else {
    serialESP.println("-1");
  }

  // 3. Logika Trigger Suara Anti-Macet
  if (distanceCm > 0 && distanceCm <= 5.0) {
    if (!isSoundTriggered) {
      Serial.println(F("TRIGGER! Memutar: Saya Akan Lawan"));
      myDFPlayer.play(1);      // Putar file /mp3/0001.mp3
      isSoundTriggered = true; // Kunci pemicu
    }
  } else if (distanceCm > 8.0) {
    // Reset kunci trigger hanya jika objek sudah menjauh di atas 8cm
    isSoundTriggered = false;
  }

  delay(100); // Loop berjalan lancar tanpa pembekuan
}