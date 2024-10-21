#include <M5Unified.h>
#include <WiFi.h>

// 设置 Wi-Fi AP 的 SSID 和密码
const char* ssid = "M5StickCPlus_AP";  //更改成你需要的名字
const char* password = "12345678"; //更改成你想要的密码

// 用于存储当前显示的状态
bool showConnectionInfo = false;

void setup() {
  // 初始化 M5StickC Plus2 和配置
  auto cfg = M5.config();
  cfg.external_imu = false; // 禁用外部IMU以提升启动速度
  M5.begin(cfg);

  // 设置屏幕方向
  M5.Display.setRotation(1); 

  // 启动 Wi-Fi AP
  WiFi.softAP(ssid, password);

  // 初始状态显示
  displayAPInfo();
}

void loop() {
  // 检查 A 按钮是否被按下
  if (M5.BtnA.wasPressed()) {
    showConnectionInfo = true;  // 切换到显示连接信息状态
    displayConnectionInfo();    // 显示连接成功和设备数量
  }

  // 检查 B 按钮是否被按下
  if (M5.BtnB.wasPressed()) {
    showConnectionInfo = false; // 切换回初始状态
    displayAPInfo();            // 显示 SSID 和 IP 信息
  }

  // 更新按钮状态
  M5.update();
}

// 显示 Wi-Fi AP 的信息：SSID 和 IP 地址
void displayAPInfo() {
  // 清屏
  M5.Display.fillScreen(BLACK);
  // 设置文本颜色和大小
  M5.Display.setTextColor(TFT_BLUE);
  M5.Display.setTextSize(2);

  // 显示 AP 信息
  M5.Display.setCursor(10, 10);
  M5.Display.println("SSID: " + String(ssid));

  IPAddress IP = WiFi.softAPIP();  // 获取 AP 的 IP 地址
  M5.Display.setCursor(10, 40);
  M5.Display.println("IP Address: ");
  M5.Display.setCursor(10, 60);
  M5.Display.println(IP);

  M5.Display.setCursor(10, 100);
  M5.Display.println("Press A for connections");
}

// 显示连接信息：连接成功和设备数量
void displayConnectionInfo() {
  // 清屏
  M5.Display.fillScreen(BLACK);
  // 设置文本颜色和大小
  M5.Display.setTextColor(TFT_GREEN);
  M5.Display.setTextSize(2);

  // 显示连接成功提示
  M5.Display.setCursor(10, 10);
  M5.Display.println("Connection Success!");

  // 获取当前连接的设备数量
  int numClients = WiFi.softAPgetStationNum();
  M5.Display.setCursor(10, 40);
  M5.Display.println("Devices connected: " + String(numClients));

  M5.Display.setCursor(10, 80);
  M5.Display.println("Press B to return");
}
