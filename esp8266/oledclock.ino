#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "PEARLHOME1";
const char* password = "mutiara2166";

int GMTOffset = 28800;  // Malaysia/Singapore time is UTC+8, so 8 hours in seconds
int daylightOffset = 0;  // No daylight savings time

const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  configTime(GMTOffset, daylightOffset, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  for (int i = 0; i < 5; i++) {
    analog();
  }
  for (int i = 0; i < 5; i++) {
    digital();
  }
}

void drawClockHand(float angle, int radius, int length, int thickness) {
  int x1 = 64 + sin(angle) * radius;
  int y1 = 40 - cos(angle) * radius;  // Adjusted y-coordinate calculation
  int x2 = 64 + sin(angle) * (radius - length);
  int y2 = 40 - cos(angle) * (radius - length);  // Adjusted y-coordinate calculation
  display.drawLine(x1, y1, x2, y2, WHITE);
}

void analog() {
  time_t rawtime = time(nullptr);
  struct tm* timeinfo = localtime(&rawtime);
  const int radius = 20;

  // Display day and date at the top
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.printf("%s %s %d, %d", days[timeinfo->tm_wday], months[timeinfo->tm_mon], timeinfo->tm_mday, timeinfo->tm_year + 1900);

  // Draw clock marks
  for (int i = 0; i < 360; i += 30) {
    float angle = i * PI / 180.0;
    drawClockHand(angle, radius, 3, 1); // Adjusted line thickness for marks
  }

  // Draw clock hands
  drawClockHand(timeinfo->tm_sec * 6 * PI / 180.0, radius, radius - 2, 1); // Thin second hand
  drawClockHand(timeinfo->tm_min * 6 * PI / 180.0, radius, radius - 5, 3); // Thick minute hand
  drawClockHand((timeinfo->tm_hour % 12 + timeinfo->tm_min / 60.0) * 30 * PI / 180.0, radius, radius - 8, 3); // Thick hour hand

  display.display();
  delay(1000);
  display.clearDisplay();
}

void digital() {
  time_t rawtime = time(nullptr);
  struct tm* timeinfo = localtime(&rawtime);

  // Display day and date at the top
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.printf("%s %s %d, %d", days[timeinfo->tm_wday], months[timeinfo->tm_mon], timeinfo->tm_mday, timeinfo->tm_year + 1900);

  // Display time with seconds
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.printf("%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

  display.display();
  delay(1000);
  display.clearDisplay();
}
