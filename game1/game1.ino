#include <M5Unified.h>

float ballX = 80;  // 将小球的初始X位置设置为屏幕中心
float ballY = 40;  // 将小球的初始Y位置设置为屏幕中心
float speed = 2.0;  // 移动速度

void setup() {
  auto cfg = M5.config();
  cfg.output_power = true;  // 启用输出电源
  M5.begin(cfg);  // 初始化M5StickC Plus2
  
  // 初始化IMU传感器
  if (!M5.Imu.begin()) {
    M5.Display.println("IMU 初始化失败！");
    while (1);  // 停止程序
  }

  M5.Display.setRotation(3);  // 设置屏幕旋转为3（横向）

  // 设置显示 "hello" 的位置和颜色
  M5.Display.setCursor(60, 70);  // 设置标题的位置（旋转后的坐标）
  M5.Display.setTextColor(GREEN);  // 设置文字颜色为绿色
  M5.Display.println("hello");  // 输出标题
}

void loop() {
  M5.update();  // 更新按钮状态，确保检测到按键

  // 检测A按钮是否按下
  if (M5.BtnA.isPressed()) {
    // 如果按下A按钮，重置小球的位置为屏幕中心
    ballX = 80;
    ballY = 40;
  }

  float ax, ay, az;
  
  // 获取IMU加速度数据
  M5.Imu.getAccel(&ax, &ay, &az);

  // 交换X和Y加速度值
  float tempX = ax;
  ax = ay;
  ay = tempX;

  // 反转Y轴方向，修正上下移动
  ay = -ay;

  // 根据加速度值移动小球位置
  ballX -= ax * speed;  // 小球在X方向根据加速度反方向移动
  ballY += ay * speed;  // 小球在Y方向根据加速度正方向移动

  // 边界检查，确保小球不会移出屏幕
  if (ballX < 0) ballX = 0;
  if (ballX > 159) ballX = 159;  // 屏幕的宽度为160像素，所以最大X坐标为159
  if (ballY < 0) ballY = 0;
  if (ballY > 79) ballY = 79;    // 屏幕的高度为80像素，所以最大Y坐标为79

  // 清除屏幕并重新绘制小球
  M5.Display.fillScreen(BLACK);  // 用黑色清屏
  M5.Display.fillCircle(ballX, ballY, 5, RED);  // 用红色重新绘制小球

  // 重新显示标题，避免被清除
  M5.Display.setCursor(60, 70);  // 标题的位置（旋转后右下角位置）
  M5.Display.setTextColor(GREEN);  // 设置文字颜色为绿色
  M5.Display.println("hello");  // 显示 "hello" 标题

  delay(50);  // 延迟50毫秒
}
