#include <M5Unified.h>

void setup() {
  // 初始化 M5StickC Plus2
  auto cfg = M5.config();  // 获取 M5StickC 配置
  M5.begin(cfg);

  // 设置屏幕方向
  M5.Lcd.setRotation(1);

  // 清空屏幕
  M5.Lcd.fillScreen(BLACK);

  // 设置文本颜色为绿色和字体大小
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);

  // 在屏幕上显示 "Hello World"
  M5.Lcd.setCursor(10, 10);  // 设置光标位置
  M5.Lcd.println("Hello World");
}

void loop() {
  // 空循环，保持显示内容
}
