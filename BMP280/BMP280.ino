#include <M5Unified.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SERIAL_BAUD 115200
Adafruit_BMP280 bmp;  // 使用 Adafruit BMP280 库对象

void setup() {
  // 初始化串口和 M5StickC Plus2
  Serial.begin(SERIAL_BAUD);
  auto cfg = M5.config();
  cfg.internal_imu = false;  // 禁用 IMU（如果不需要）
  M5.begin(cfg);

  // 初始化 BMP280 传感器
  if (!bmp.begin(0x76)) {  // 尝试使用 I2C 地址 0x76 初始化
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    M5.Lcd.println("BMP280 not found!");
    while (1);
  } else {
    Serial.println("BMP280 sensor initialized.");
    M5.Lcd.println("BMP280 initialized.");
  }

  // 设置屏幕方向（可选：0, 1, 2, 3）
  M5.Lcd.setRotation(3);  // 调整方向，使屏幕横向显示
}

void loop() {
  // 读取温度和气压
  float temp = bmp.readTemperature();
  float pres = bmp.readPressure() / 100.0F;  // 转换为 hPa

  // 输出到串口监视器
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pres);
  Serial.println(" hPa");

  // 在 M5StickC Plus2 上显示数据
  M5.Lcd.fillScreen(TFT_BLACK);  // 清屏
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(TFT_GREEN);  // 设置文本为绿色
  M5.Lcd.printf("Temp: %.2f C\n", temp);
  M5.Lcd.printf("Pressure: %.2f hPa\n", pres);

  delay(5000);  // 每 5 秒更新一次数据
}
