#include <M5Unified.h>
#include <Wire.h>
#include <BME280I2C.h>

#define SERIAL_BAUD 115200

BME280I2C bme;

void setup() {
  M5.begin();
  Serial.begin(SERIAL_BAUD);
  
  Wire.begin();

  M5.Display.setRotation(3); // 设置屏幕方向，视情况而定
  M5.Display.fillScreen(BLACK); // 清屏

  // 初始化 BME280 传感器
  if (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    M5.Display.setCursor(0, 10);
    M5.Display.print("BME280 not found!");
    delay(1000);
    M5.Display.fillScreen(BLACK); // 清屏
    return; // 如果传感器未找到，停止程序
  }

  M5.Display.setCursor(0, 10);
  M5.Display.print("BME280 found!");
  delay(1000);
  M5.Display.fillScreen(BLACK); // 清屏
}

void loop() {
  float temp, hum, pres;
  BME280::TempUnit tu(BME280::TempUnit_Celsius);
  BME280::PresUnit pu(BME280::PresUnit_hPa);

  // 读取传感器数据
  bme.read(pres, temp, hum, tu, pu);

  // 在串口监视器上打印数据
  Serial.print("Sec: " + String(millis() / 1000));
  Serial.print("\tTemp: " + String(temp) + "°C");
  Serial.print("\tHumidity: " + String(hum) + "% RH");
  Serial.println("\tPressure: " + String(pres) + " hPa");

  // 在 M5StickC Plus2 上显示数据
  M5.Display.setCursor(0, 30);
  M5.Display.fillRect(0, 30, 160, 60, BLACK); // 清除之前的数据显示区域
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  M5.Display.print("Temp: ");
  M5.Display.print(temp);
  M5.Display.print(" °C");
  
  M5.Display.setCursor(0, 60);
  M5.Display.fillRect(0, 60, 160, 60, BLACK); // 清除之前的数据显示区域
  M5.Display.print("Humidity: ");
  M5.Display.print(hum);
  M5.Display.print(" % RH");
  
  M5.Display.setCursor(0, 90);
  M5.Display.fillRect(0, 90, 160, 60,BLACK); // 清除之前的数据显示区域
  M5.Display.print("Pressure: ");
  M5.Display.print(pres);
  M5.Display.print(" hPa");

  delay(5000); // 每 5 秒更新一次数据
}
