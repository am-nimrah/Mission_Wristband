#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <DHT.h>
#include <EEPROM.h>
#include <LedControl.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Dot Matrix (DIN, CLK, CS)
LedControl lc = LedControl(11, 13, 10, 1); // Only 1 device

// Sensor & Pin Setup
#define PIR_PIN 2
#define SOIL_PIN A0
#define LDR_PIN A1
#define DHT_PIN 3
#define BTN_PIN 4
#define BUZZER_PIN 5

#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Emoji patterns
byte heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

byte leaf[8] = {
  B00011000,
  B00111100,
  B00011000,
  B00011000,
  B00111100,
  B01100110,
  B00011000,
  B00000000
};

byte bulb[8] = {
  B00011000,
  B00111100,
  B00111100,
  B00111100,
  B00011000,
  B00011000,
  B00111100,
  B00000000
};

byte smiley[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// State
int currentMission = -1;
int score = 0;

void setup() {
  Serial.begin(9600);

  // Pin modes
  pinMode(PIR_PIN, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start components
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  dht.begin();

  lc.shutdown(0, false);      // Wake up dot matrix
  lc.setIntensity(0, 8);      // Brightness
  lc.clearDisplay(0);

  EEPROM.get(0, score);       // Load score from EEPROM

  showMessage("World Saver!", "Loading...");
  displayEmoji(heart);
  delay(2000);
}

void loop() {
  int pir = digitalRead(PIR_PIN);
  int soil = analogRead(SOIL_PIN);
  int light = analogRead(LDR_PIN);

  // Mission Conditions
  if (soil < 300) {
    currentMission = 0;
    showMission("Water Plant");
    displayEmoji(leaf);
  } else if (pir == LOW && light > 600) {
    currentMission = 1;
    showMission("Turn off Light");
    displayEmoji(bulb);
  } else {
    currentMission = -1;
    showMessage("No Mission", "All good!");
    displayEmoji(smiley);
  }

  // Mission Complete Button
  if (digitalRead(BTN_PIN) == LOW && currentMission != -1) {
    score += 10;
    EEPROM.put(0, score);
    buzzSuccess();
    showMessage("Mission Done!", "Points: " + String(score));
    displayEmoji(heart);
    delay(2000);
  }

  delay(5000); // Recheck after delay
}

void showMission(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("New Mission:");
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.println(msg);
  display.display();
}

void showMessage(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}

void buzzSuccess() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

void displayEmoji(byte emoji[8]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, emoji[i]);
  }
}
