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
int angle = 0;  // มุมเริ่มต้นของเซอร์โวมอเตอร์
int speed = 15;  // กำหนดความเร็วในการหมุน (ยิ่งค่าน้อย ยิ่งเร็ว)

// ฟังก์ชันควบคุมการหมุนเซอร์โวมอเตอร์ตามค่าที่ได้จาก Blynk
void control_servo(int target_angle) {
  if (angle < target_angle) {
    // หมุนเซอร์โวจากมุมปัจจุบันไปยังมุมที่ต้องการ
    for (int pos = angle; pos <= target_angle; pos += 1) { 
      myServo.write(pos);
      delay(speed);  // กำหนดความเร็วในการหมุน
    }
  } else if (angle > target_angle) {
    // หมุนเซอร์โวกลับจากมุมปัจจุบันไปยังมุมที่ต้องการ
    for (int pos = angle; pos >= target_angle; pos -= 1) {
      myServo.write(pos);
      delay(speed);  // กำหนดความเร็วในการหมุน
    }
  }
  angle = target_angle;  // บันทึกมุมปัจจุบันเพื่อใช้เป็นค่าเริ่มต้นครั้งต่อไป
}

void setup() {
  Serial.begin(115200);  // เปิด Serial Monitor ที่ baud rate 115200
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);  // เริ่มการเชื่อมต่อกับ Blynk
  myServo.attach(servoPin);  // เชื่อมต่อเซอร์โวมอเตอร์ที่พอร์ต D4
  myServo.write(angle);  // ตั้งค่าเริ่มต้นของเซอร์โวมอเตอร์
}

BLYNK_WRITE(V0) {  // รับค่าจาก Virtual Pin V0 ใน Blynk
  int target_angle = param.asInt();  // อ่านค่ามุมจากแอป Blynk
  Serial.println(target_angle);  // พิมพ์มุมที่ได้รับออกที่ Serial Monitor
  control_servo(target_angle);  // เรียกฟังก์ชันเพื่อควบคุมเซอร์โวมอเตอร์
}

BLYNK_WRITE(V1) {  // รับค่าความเร็วในการหมุนจาก Virtual Pin V1
  speed = param.asInt();  // อ่านค่าความเร็วจากแอป Blynk
  Serial.print("Speed set to: ");
  Serial.println(speed);  // พิมพ์ความเร็วที่ได้รับออกที่ Serial Monitor
}

void loop() {
  Blynk.run();  // เริ่มการทำงานของ Blynk
}
