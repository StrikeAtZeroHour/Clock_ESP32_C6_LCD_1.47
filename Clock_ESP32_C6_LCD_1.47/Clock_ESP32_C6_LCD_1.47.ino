#include <WiFi.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <SPI.h>

// WiFi credentials
#define WIFI_SSID "chanwifi"
#define WIFI_PASSWORD "90362439"
#define WIFI_CHANNEL 6

// NTP server settings
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET (8 * 3600) // UTC offset in seconds (0 for UTC)
#define UTC_OFFSET_DST 0 // Daylight saving time offset in seconds

// SPI pins
#define TFT_CS    14
#define TFT_DC    15
#define TFT_RST   21
#define TFT_MOSI  6
#define TFT_SCK   7
// Backlight pin (optional, comment out if not used)
#define TFT_BL    22 

// Colors (RGB565 format)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

// LCD dimensions
#define LCD_WIDTH  172
#define LCD_HEIGHT 320

// Initialize ST7789 display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

void setup() {
  Serial.begin(115200);

  // Initialize backlight (if used)
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight
#endif

  // Initialize LCD
  tft.init(LCD_WIDTH, LCD_HEIGHT);
  Serial.println("LCD Init Done");
  tft.setRotation(0); // 0 for VERTICAL, 1 for HORIZONTAL
  tft.fillScreen(COLOR_BLACK); // Clear screen

  // Test full-screen fill
  tft.fillScreen(COLOR_WHITE);
  Serial.println("Filled screen white");
  delay(2000); // Show white screen for 2 seconds
  tft.fillScreen(COLOR_BLACK);
  Serial.println("Cleared screen to black");

  // Configure text
  tft.setFont(&FreeMonoBold12pt7b); // Large font
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK); // White text, black background
  tft.setTextSize(1); // 2x scale (~24-30 pixels)

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Configure time with NTP server
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  Serial.println("Synchronizing time...");
  tft.setCursor(0, LCD_HEIGHT/4);
  tft.print("Current Time");
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Format time string (HH:MM:SS)
  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  Serial.println(timeStr);

  // Calculate text bounds for centering
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  Serial.printf("Text width: %d, height: %d\n", w, h);

  // Clear previous text area
  int16_t textX = (LCD_WIDTH - w) / 2;
  int16_t textY = (LCD_HEIGHT - h) / 2 + h; // Adjust for baseline
  tft.fillRect(textX, textY - h, w+sizeof(timeStr), h+sizeof(timeStr), COLOR_BLACK);

  // Draw new time string
  tft.setCursor(textX, textY);
  tft.print(timeStr);

  delay(1000); // Update every second
}
