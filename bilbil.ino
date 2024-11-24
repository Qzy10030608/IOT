#include <M5Unified.h>
#include <math.h>

#define BUZZER_PIN 2           // 蜂鸣器引脚
#define FALL_THRESHOLD 3.0     // 默认加速度阈值
#define ALARM_DURATION 2000    // 警报持续时间（毫秒）

unsigned long alarmStartTime = 0; // 警报开始时间
bool isAlarmActive = false;       // 是否处于警报状态

void setup() {
  M5.begin();
  M5.Display.setRotation(3);
  M5.Display.fillScreen(BLACK);

  // 显示永久数字 "123"
  M5.Display.setTextSize(4);      // 设置大字体
  M5.Display.setTextColor(BLUE);  // 设置蓝色字体
  M5.Display.setCursor(30, 30);   // 设置显示位置
  M5.Display.println("123");

  // 配置蜂鸣器
  ledcSetup(0, 2000, 8);         // 通道 0，频率 2000Hz，分辨率 8 位
  ledcAttachPin(BUZZER_PIN, 0);  // 将通道 0 绑定到 BUZZER_PIN
}

void loop() {
  M5.update();

  // 动态调整阈值
  static float fallThreshold = FALL_THRESHOLD;
  if (M5.BtnA.wasPressed()) {
    fallThreshold += 1.0; // 增大阈值
    M5.Display.fillScreen(BLACK);
    // 显示永久数字 "123"
    M5.Display.setTextSize(5);
    M5.Display.setTextColor(BLUE);
    M5.Display.setCursor(10, 10);
    M5.Display.println("123");
    // 显示新的阈值
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE);
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Threshold: %.2f", fallThreshold);
  }
  if (M5.BtnB.wasPressed()) {
    fallThreshold -= 1.0; // 减小阈值
    if (fallThreshold < 1.0) fallThreshold = 1.0;
    M5.Display.fillScreen(BLACK);
    // 显示永久数字 "123"
    M5.Display.setTextSize(4);
    M5.Display.setTextColor(BLUE);
    M5.Display.setCursor(30, 30);
    M5.Display.println("123");
    // 显示新的阈值
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE);
    M5.Display.setCursor(10, 100);
    M5.Display.printf("Threshold: %.2f", fallThreshold);
  }

  // 获取加速度数据
  float ax, ay, az;
  M5.Imu.getAccel(&ax, &ay, &az);

  // 计算加速度的大小
  float magnitude = sqrt(ax * ax + ay * ay + az * az);

  if (magnitude > fallThreshold) {
    if (!isAlarmActive) {
      // 检测到超过阈值，启动报警
      isAlarmActive = true;
      alarmStartTime = millis();

      // 红色背景
      M5.Display.fillScreen(RED);
      // 保留数字 "123"
      M5.Display.setTextSize(4);
      M5.Display.setTextColor(BLUE);
      M5.Display.setCursor(30, 30);
      M5.Display.println("123");

      // 显示绿色警报文字
      M5.Display.setTextSize(3);
      M5.Display.setTextColor(GREEN);
      M5.Display.setCursor(10, 200);
      M5.Display.println("FALL DETECTED!");

      // 启动蜂鸣器
      ledcWrite(0, 128); // 启动蜂鸣器 50% 占空比
    }
  }

  // 如果警报已启动，检查是否需要停止
  if (isAlarmActive && (millis() - alarmStartTime > ALARM_DURATION)) {
    // 停止警报
    isAlarmActive = false;
    ledcWrite(0, 0); // 关闭蜂鸣器
    M5.Display.fillScreen(BLACK);

    // 重新显示永久数字 "123"
    M5.Display.setTextSize(4);
    M5.Display.setTextColor(BLUE);
    M5.Display.setCursor(30, 30);
    M5.Display.println("123");
  }

  delay(50); // 循环间隔
}
