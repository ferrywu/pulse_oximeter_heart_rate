#include <Adafruit_GFX.h>        // OLED libraries
#include <Adafruit_SSD1306.h>
#include "ESP32Servo.h"

// OLED Settings
#define SCREEN_WIDTH 128 // OLED width
#define SCREEN_HEIGHT 64 // OLED height
#define OLED_RESET    -1 // Reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declaring the display name (display)

// Tone Settings
int Tonepin = 4;

// heart beat small logo
static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,              // Logo2 and Logo3 are two bmp pictures that display on the OLED if called
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,
};

// heart beat large logo
static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

// oxygen logo
static const unsigned char PROGMEM O2_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x3f, 0xc3, 0xf8, 0x00, 0xff, 0xf3, 0xfc,
  0x03, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0x7e,
  0x1f, 0x80, 0xff, 0xfc, 0x1f, 0x00, 0x7f, 0xb8, 0x3e, 0x3e, 0x3f, 0xb0, 0x3e, 0x3f, 0x3f, 0xc0,
  0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3f, 0x1f, 0xc0, 0x3e, 0x3e, 0x2f, 0xc0,
  0x3e, 0x3f, 0x0f, 0x80, 0x1f, 0x1c, 0x2f, 0x80, 0x1f, 0x80, 0xcf, 0x80, 0x1f, 0xe3, 0x9f, 0x00,
  0x0f, 0xff, 0x3f, 0x00, 0x07, 0xfe, 0xfe, 0x00, 0x0b, 0xfe, 0x0c, 0x00, 0x1d, 0xff, 0xf8, 0x00,
  0x1e, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00,
  0x0f, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void display_init(void) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Start the OLED display
  display.display();
}

void display_wifi_connecting(const char *ssid) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("Connecting to ");
  display.setCursor(10, 40);
  display.print(ssid);
  display.display();
  Serial.println("");
  Serial.println("Connecting to ");
  Serial.print(ssid);
}

void display_wifi_progress(void) {
  display.print(".");
  display.display();
  Serial.print(".");
}

void display_wifi_connected(const char *ip) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 15);
  display.println("WiFi connected");
  display.setCursor(10, 30);
  display.println("IP address: ");
  display.setCursor(10, 45);
  display.println(ip);
  display.display();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(ip);
}

void display_oximeter_info(bool isBeat, int beatAvg, double ESpO2) {
  display.clearDisplay();
  if (isBeat)
    display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE); // display large heart beat logo
  else
    display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE); // display small heart beat logo
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(42, 10);
  display.print(beatAvg); display.println(" BPM"); // display hearbeat value
  display.drawBitmap(0, 35, O2_bmp, 32, 32, WHITE); // display oxygen logo
  display.setCursor(42, 40);
  // display pulse oximeter value
  if (beatAvg > 30) display.print(String(ESpO2) + "%");
  else display.print("---- %" );
  display.display();
  if (isBeat) {
    tone(Tonepin, 1000); // start tone
    delay(10);
    noTone(Tonepin); // stop tone
    Serial.print("beatAvg="); Serial.println(beatAvg); // send heart beat value to serial port
  }
}

void display_wait_for_finger(void) {
  // display Finger Please
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 5);
  display.println("Finger");
  display.setCursor(30, 35);
  display.println("Please");
  display.display();
  noTone(Tonepin);
}
