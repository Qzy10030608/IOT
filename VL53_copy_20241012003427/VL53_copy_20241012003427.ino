#include <M5Unified.h>  // 引入 M5Unified 库
#include <Wire.h>
#include <VL53L0X.h>    // 引入 VL53L0X 传感器库

VL53L0X sensor;

void setup() {
  // 初始化 M5StickPlus2 和显示屏
  auto cfg = M5.config();
  cfg.output_power = true;  // 打开电源
  M5.begin(cfg);
  M5.Display.setRotation(3);  // 设置屏幕显示方向
  M5.Display.clearDisplay();  // 清屏
  M5.Display.setTextSize(2);  // 设置字体大小

  // 初始化 I2C 和 VL53L0X 传感器
  Wire.begin();
  sensor.setTimeout(500);

  if (!sensor.init()) {
    M5.Display.print("Sensor init failed!");  // 初始化失败，显示错误信息
    while (1);
  }

  sensor.startContinuous();  // 开始连续测距
}

void loop() {
  // 清除上一轮显示的内容
  M5.Display.clearDisplay();

  // 读取 VL53L0X 传感器的距离数据
  int distance = sensor.readRangeContinuousMillimeters();

  // 检查传感器是否超时
  if (sensor.timeoutOccurred()) {
    M5.Display.setCursor(10, 40);  // 设置显示位置
    M5.Display.print("Timeout!");
  } else {
    // 在屏幕上显示测量距离
    M5.Display.setCursor(10, 40);
    M5.Display.printf("Distance: %d mm", distance);
  }

  // 延时 500 毫秒
  delay(500);
}

