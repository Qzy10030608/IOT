#include <M5Unified.h>    // 使用 M5StickC Plus 2 的库
#include "MHZ19.h"

#define RX_PIN 36                                          // MHZ19的Rx引脚
#define TX_PIN 26                                          // MHZ19的Tx引脚
#define BAUDRATE 9600                                      // MH-Z19默认波特率

#define LED_PIN 19                                         // LED 引脚
#define LCD_MODE_DIGIT 0                                   // 数字模式
#define LCD_MODE_GRAPH 1                                   // 图形模式

#define BRIGHTNESS 8                                       // 屏幕亮度

MHZ19 myMHZ19;                                             // MH-Z19 库对象
HardwareSerial mySerial(1);                                // 串口通信对象

bool lcdOn = true;                                         // 屏幕状态
int lcdMode = LCD_MODE_DIGIT;                             // 当前显示模式
int history = 0;                                          // 存储最近的 CO2 数据
unsigned long getDataTimer = 0;                          // 数据读取定时器

void setup() {
  M5.begin();                                             // 初始化 M5StickC Plus 2
  M5.Display.setBrightness(BRIGHTNESS);                   // 设置屏幕亮度

  Serial.begin(9600);                                     // 串口监视器，用于调试输出
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);  // 初始化MH-Z19传感器的串口
  myMHZ19.begin(mySerial);                                // 初始化MH-Z19库
  myMHZ19.autoCalibration(true);                          // 开启自动校准

  M5.Display.setRotation(1);                              // 设置屏幕方向
  render();                                               // 渲染初始显示

  pinMode(LED_PIN, OUTPUT);                               // 初始化 LED 引脚
}

void loop() {
  auto now = millis();
  M5.update();                                            // 更新按键状态

  // 切换显示模式
  if (M5.BtnA.wasPressed()) {
    lcdMode = (lcdMode + 1) % 2;
    render();
  }

  // 控制屏幕开关
  if (M5.BtnB.wasPressed()) {
    lcdOn = !lcdOn;
    M5.Display.setBrightness(lcdOn ? BRIGHTNESS : 0);
    if (lcdOn) render();                                  // 恢复显示
  }

  // 每15秒读取一次 CO2 数据
  if (now - getDataTimer >= 30000) {                     // 简单的定时器逻辑
    int CO2 = myMHZ19.getCO2();                          // 获取 CO2 浓度
    Serial.print("CO2 (ppm): ");
    Serial.println(CO2);
    
    // LED控制：如果CO2浓度超过1200ppm，开启LED
    digitalWrite(LED_PIN, CO2 >= 1200 ? HIGH : LOW);
    
    history = CO2;                                       // 更新最近的 CO2 数据
    render();                                            // 更新显示

    getDataTimer = now;                                  // 重置定时器
  }
}

void render() {
  if (!lcdOn) return;                                    // 如果屏幕关闭，直接返回

  // 清除屏幕
  M5.Display.fillRect(0, 0, M5.Display.width(), M5.Display.height(), BLACK);
  M5.Display.setTextSize(2);
  
  switch (lcdMode) {
    case LCD_MODE_DIGIT:
      M5.Display.drawString("CO2 [ppm]", 12, 0, 2);
      M5.Display.drawRightString(String(history), M5.Display.width(), 24, 7);
      break;
    case LCD_MODE_GRAPH:
      // 可根据需求添加图形显示代码
      break;
  }
}
