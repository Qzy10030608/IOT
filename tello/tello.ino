#define LOAD_FONT2
#define LOAD_FONT4
#include <M5Unified.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char* TELLO_IP = "192.168.10.1";  // Tello无人机的IP地址
const char* TELLO_SSID = "TELLO-CCDA6F";  // Tello的SSID名称
const char* TELLO_No = "02";  // 无人机编号
const int PORT = 8889;  // Tello控制端口号

WiFiUDP Udp;  // 创建UDP对象用于通信
char packetBuffer[255];  // 数据包缓冲区
String message = "";  // 接收的消息

float x;
float y;

char msgx[6];
char msgy[6];

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float accX_sum = 0.0F;
float accY_sum = 0.0F;
float accX_diff = 0.0F;
float accY_diff = 0.0F;
int count;

unsigned long lastSendTime = 0;  // 上次发送时间
const unsigned long sendInterval = 1000;  // 发送命令的间隔时间
int commandCount = 0;  // 发送命令的次数
const int maxCommands = 10;  // 最大发送命令次数

void setup() {
  auto cfg = M5.config();
  cfg.internal_imu = true;  // 启用内置IMU传感器
  M5.begin(cfg);  // 初始化M5设备
  M5.Imu.begin();  // 启动IMU传感器
  
  // 显示标题区域，红色背景表示未连接
  M5.Lcd.fillRect(0, 0, 80, 20, TFT_RED);  
  M5.Lcd.setTextSize(2);  // 设置默认字体大小为2
  M5.Lcd.drawCentreString(TELLO_No, 40, 2, 1);  // 显示无人机编号
  M5.Lcd.drawCentreString("Controller", 40, 10, 1);

  // 方向指示器的三角形背景
  M5.Lcd.fillTriangle(20, 50, 40, 30, 60, 50, TFT_WHITE);
  M5.Lcd.fillTriangle(20, 50, 40, 70, 60, 50, TFT_WHITE);

  // 方向文字
  M5.Lcd.setTextColor(TFT_BLACK, TFT_YELLOW);
  updateDirectionDisplay(0, 0);  // 初始显示方向指示器

  // 初始化显示加速度数据
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.drawCentreString("accX: ", 20, 110, 1);
  M5.Lcd.drawCentreString("accY: ", 20, 130, 1);

  // 获取加速度的补偿值
  for (count = 1; count <= 10; count++) {
    M5.Imu.getAccel(&accX, &accY, &accZ);
    delay(20);
    accX_sum += accX;
    accY_sum += accY;
  }
  accX_diff = accX_sum / 10;  // 计算X轴补偿值
  accY_diff = accY_sum / 10;  // 计算Y轴补偿值

  // 初始化WiFi连接
  WiFi.begin(TELLO_SSID, "");  // Tello无需密码
  while (WiFi.status() != WL_CONNECTED) {
    print_msg("Connecting to WiFi..."); // 使用英文显示 WiFi 连接信息
    delay(50);
  }
  
  print_msg("WiFi连接成功");
  Udp.begin(PORT);  // 启动UDP通信
  tello_command_exec("command");  // 向Tello发送“command”指令进入命令模式
  delay(50);

  // 连接成功后将标题区域背景颜色改为绿色
  M5.Lcd.fillRect(0, 0, 80, 20, TFT_GREEN);
  M5.Lcd.drawCentreString(TELLO_No, 40, 2, 1);
  M5.Lcd.drawCentreString("Controller", 40, 10, 1);
}

void loop() {
  M5.Imu.getAccel(&accX, &accY, &accZ);  // 获取当前加速度数据
  x = accX - accX_diff;  // 加X补偿
  y = accY - accY_diff;  // 加Y补偿

  // 显示X轴加速度
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  sprintf(msgx, "%-2.2f", x);
  M5.Lcd.drawCentreString(msgx, 60, 110, 1);

  // 显示Y轴加速度
  sprintf(msgy, "%-2.2f", y);
  M5.Lcd.drawCentreString(msgy, 60, 130, 1);

  // 调整方向指示器显示
  updateDirectionDisplay(x, y);

  // 单按B键起飞
  if (M5.BtnB.wasPressed()) {
    print_msg("起飞");
    tello_command_exec("takeoff");
    lastSendTime = millis();  // 记录发送时间
    commandCount++;  // 增加命令计数
    delay(500);
  }

  // 长按B键降落
  if (M5.BtnB.pressedFor(300)) {
    print_msg("降落");
    tello_command_exec("land");
    lastSendTime = millis();  // 记录发送时间
    commandCount++;  // 增加命令计数
    delay(500);
  }

  // 按A键根据加速度执行动作
  if (M5.BtnA.isPressed()) {
    // 旋转45度
    print_msg("旋转45度");
    tello_command_exec("ccw 45");  // 逆时针旋转
    lastSendTime = millis();  // 记录发送时间
    commandCount++;  // 增加命令计数
    delay(500);
  }

  // 根据加速度执行高度变化
  if (M5.BtnA.pressedFor(300)) {
    if (y > 0.5) {
      print_msg("上升");
      tello_command_exec("up 50");  // 上升50cm
      lastSendTime = millis();  // 记录发送时间
      commandCount++;  // 增加命令计数
    } else if (y < -0.5) {
      print_msg("下降");
      tello_command_exec("down 50");  // 下降50cm
      lastSendTime = millis();  // 记录发送时间
      commandCount++;  // 增加命令计数
    }
  }

  // 根据X和Y的值执行左右和前后移动
  if (millis() - lastSendTime >= sendInterval && commandCount < maxCommands) {
    if (fabs(x) > 0.4) {
      if (x < 0) {  // 左移
        print_msg("左移");
        tello_command_exec("right 30");
      } else {  // 右移
        print_msg("右移");
        tello_command_exec("left 30");
      }
      lastSendTime = millis();  // 记录发送时间
      commandCount++;  // 增加命令计数
    }
    
    if (fabs(y) > 0.4) {
      if (y < 0) {  // 前进
        print_msg("前进");
        tello_command_exec("back 30");
      } else {  // 后退
        print_msg("后退");
        tello_command_exec("forward 30");
      }
      lastSendTime = millis();  // 记录发送时间
      commandCount++;  // 增加命令计数
    }
  }

  // 当发送的命令达到最大次数时，停止执行
  if (commandCount >= maxCommands) {
    print_msg("达到最大命令数，停止发送");
    commandCount = 0;  // 重置命令计数
  }

  M5.update();  // 更新M5的状态
}

// 更新方向显示
void updateDirectionDisplay(float x, float y) {
  M5.Lcd.setTextColor(TFT_BLACK, TFT_YELLOW);
  M5.Lcd.drawCentreString("F", 50, 20, 2);  // 前进
  M5.Lcd.drawCentreString("B", 50, 80, 2);  // 后退
  M5.Lcd.drawCentreString("L", 10, 50, 2);  // 左移
  M5.Lcd.drawCentreString("R", 90, 50, 2);  // 右移
}

// 向Tello发送命令
void tello_command_exec(const char* command) {
  Udp.beginPacket(TELLO_IP, PORT);  // 准备发送数据包
  Udp.write((const uint8_t*)command, strlen(command));  // 写入命令（转换为字节数组）
  Udp.endPacket();  // 结束数据包发送
}

// 打印信息
void print_msg(String msg) {
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.fillRect(0, 160, 160, 20, TFT_BLACK);  // 清除之前的消息区域
  M5.Lcd.drawString(msg, 0, 160);  // 显示消息
}
