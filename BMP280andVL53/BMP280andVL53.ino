#include <M5Unified.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <VL53L0X.h>

#define SERIAL_BAUD 115200
Adafruit_BMP280 bmp;  // BMP280 传感器对象
VL53L0X vl53;         // VL53 距离传感器对象

bool showDistance = true;  // 当前显示模式，默认为距离
unsigned long lastBPressTime = 0;  // B按钮按下的时间
bool stopMeasurement = false;  // 是否停止测量
unsigned long lastDistanceRefreshTime = 0;  // 距离刷新时间
unsigned long lastTempPressRefreshTime = 0; // 温度和气压刷新时间
bool screenCleared = false; // 记录屏幕是否已经清空

void setup() {
  // 初始化串口和 M5StickC Plus2
  Serial.begin(SERIAL_BAUD);
  auto cfg = M5.config();
  cfg.internal_imu = false;  // 禁用 IMU（如果不需要）
  M5.begin(cfg);

  // 初始化 BMP280 传感器
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    M5.Lcd.println("BMP280 not found!");
    while (1);
  } else {
    Serial.println("BMP280 sensor initialized.");
    M5.Lcd.println("BMP280 initialized.");
  }

  // 初始化 VL53 距离传感器
  Wire.begin();
  if (!vl53.init()) {
    Serial.println("Failed to initialize VL53 sensor!");
    M5.Lcd.println("VL53 init failed!");
    while (1);
  } else {
    Serial.println("VL53 sensor initialized.");
    M5.Lcd.println("VL53 initialized.");
  }

  vl53.setTimeout(500);
  vl53.startContinuous();

  // 设置屏幕方向
  M5.Lcd.setRotation(3);
}

void loop() {
  M5.update();  // 检查按钮状态
  unsigned long currentTime = millis();  // 当前时间

  // A按钮切换显示模式
  if (M5.BtnA.wasPressed()) {
    showDistance = !showDistance;  // 切换显示模式
    screenCleared = false; // 切换时重新刷新屏幕
  }

  // B按钮长按停止测量
  if (M5.BtnB.isPressed()) {
    if (currentTime - lastBPressTime > 3000) {  // 按下B按钮3秒
      stopMeasurement = true;  // 停止测量
    }
  } else {
    lastBPressTime = currentTime;
  }

  if (stopMeasurement) {
    if (!screenCleared) {
      M5.Lcd.fillScreen(TFT_BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(TFT_RED);
      M5.Lcd.println("Measurement Stopped");
      screenCleared = true;  // 避免重复刷新停止屏幕
    }
    return;  // 停止所有检测
  }

  // 显示内容更新控制
  if (showDistance) {
    // 每1秒刷新一次距离测量
    if (currentTime - lastDistanceRefreshTime >= 1000) {
      M5.Lcd.fillScreen(TFT_BLACK);  // 只在每次更新时清屏
      uint16_t distance = vl53.readRangeContinuousMillimeters();
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(TFT_BLUE);
      if (vl53.timeoutOccurred()) {
        M5.Lcd.println("Distance Timeout!");
      } else {
        M5.Lcd.printf("Distance: %d mm\n", distance);
      }
      lastDistanceRefreshTime = currentTime;  // 更新距离刷新时间
      screenCleared = false;  // 每次更新完屏幕标记为未清除状态
    }
  } else {
    // 每10秒刷新一次温度和气压
    if (currentTime - lastTempPressRefreshTime >= 10000) {
      M5.Lcd.fillScreen(TFT_BLACK);  // 只在每次更新时清屏
      float temp = bmp.readTemperature();
      float pres = bmp.readPressure() / 100.0F;  // 转换为 hPa
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setTextColor(TFT_GREEN);
      M5.Lcd.printf("Temp: %.2f C\n", temp);
      M5.Lcd.printf("Pressure: %.2f hPa\n", pres);
      lastTempPressRefreshTime = currentTime;  // 更新温度和气压刷新时间
      screenCleared = false;  // 每次更新完屏幕标记为未清除状态
    }
  }
}
