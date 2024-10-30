#include <M5Unified.h>
#include <WiFiManager.h>
#include <time.h>

// NTP 服务器和时区设置
const char* ntpServer = "pool.ntp.org"; // NTP 服务器地址
const long gmtOffset_sec = 9 * 3600;    // UTC+9 时区（日本标准时间）
const int daylightOffset_sec = 0;       // 无夏令时

void setup() {
  // 初始化 M5StickC Plus2
  M5.begin();
  
  // 设置为横屏模式
  M5.Lcd.setRotation(1);
  
  // 使用 WiFiManager 自动连接 WiFi
  WiFiManager wifiManager;
  wifiManager.autoConnect("M5StickCPlus2_AP");

  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println("Failed to connect to WiFi");
    return;
  }

  M5.Lcd.println("WiFi Connected!");

  // NTP 时间同步
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // 尝试获取时间
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    M5.Lcd.println("Failed to obtain time");
  } else {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextSize(3); // 设置文本大小
    M5.Lcd.setCursor(0, 20);
    
    M5.Lcd.setTextColor(TFT_BLUE);
    M5.Lcd.printf("Time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.setTextColor(TFT_PINK);
    M5.Lcd.printf("Date: %04d-%02d-%02d", (timeinfo.tm_year + 1900), timeinfo.tm_mon + 1, timeinfo.tm_mday);
  }
}

void loop() {
  struct tm timeinfo;

  // 更新显示
  if (getLocalTime(&timeinfo)) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextSize(3); // 设置文本大小
    
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.setTextColor(TFT_BLUE);
    M5.Lcd.printf("Time:%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    M5.Lcd.setTextSize(4); // 设置文本大小
    M5.Lcd.setCursor(0, 70);
    M5.Lcd.setTextColor(TFT_PURPLE);
    M5.Lcd.printf("Date: %04d-%02d-%02d", (timeinfo.tm_year + 1900), timeinfo.tm_mon + 1, timeinfo.tm_mday);
  } else {
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.setTextColor(TFT_RED); // 如果未能获取时间，则显示红色错误信息
    M5.Lcd.println("Failed to obtain time");
  }

  delay(1000); // 每秒更新一次显示
}

