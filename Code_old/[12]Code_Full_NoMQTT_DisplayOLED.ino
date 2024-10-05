// Define Blynk template ID and name at the very top
#define BLYNK_TEMPLATE_ID "TMPL65HBVGn3o"
#define BLYNK_TEMPLATE_NAME "HC04AndSevor"
#define BLYNK_AUTH_TOKEN "ue2aKWfmhmBe02cDLVOby5JJyw7_19xJ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>  // ไลบรารีสำหรับเซอร์โวมอเตอร์

// Wi-Fi credentials
const char* ssid = "HUAWEI_2310"; // Wi-Fi name
const char* password = "1234567890"; // Wi-Fi password

Servo myServo;  // สร้างอ็อบเจกต์เซอร์โว
int servoPin = D4;  // กำหนดพอร์ต D4 (GPIO2) สำหรับเซอร์โวมอเตอร์

void setup() {
  Serial.begin(115200);  // เปิด Serial Monitor ที่ baud rate 115200
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);  // เริ่มการเชื่อมต่อกับ Blynk
  myServo.attach(servoPin);       // เชื่อมต่อเซอร์โวที่พอร์ต D4
}

BLYNK_WRITE(V0) {  // รับค่าจาก Virtual Pin V0
  int angle = param.asInt();  // อ่านค่ามุมจากแอป Blynk
  Serial.println(angle);  // พิมพ์ค่า angle ออกมาที่ Serial Monitor
  myServo.write(angle);  // ปรับมุมเซอร์โวตามค่าที่รับมา
}

void loop() {
  Blynk.run();  // เริ่มการทำงานของ Blynk
}