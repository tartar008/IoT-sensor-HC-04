#include <Servo.h>

Servo myservo;  // สร้างออบเจ็กต์เซอร์โว

int servoPin = D3;  // กำหนดให้พอร์ต D3 ใช้ควบคุมเซอร์โวมอเตอร์
int angle = 0;      // กำหนดมุมเริ่มต้นที่ 0 องศา



void setup() {
  myservo.attach(servoPin);  // เชื่อมต่อเซอร์โวมอเตอร์เข้ากับพอร์ตที่กำหนด
}

void loop() {
  control_servo();
}

// ฟังก์ชันควบคุมการหมุนของ Servo
void control_servo() {
  // หมุนเซอร์โวมอเตอร์จาก 0 ถึง 180 องศา
  for (angle = 0; angle <= 180; angle += 1) { 
    myservo.write(angle);    // สั่งให้เซอร์โวมอเตอร์หมุนไปที่มุม angle
    delay(15);               // รอให้เซอร์โวมอเตอร์หมุนเสร็จ
  }
  
  // หมุนเซอร์โวมอเตอร์กลับจาก 180 ถึง 0 องศา
  for (angle = 180; angle >= 0; angle -= 1) {
    myservo.write(angle);    // สั่งให้เซอร์โวมอเตอร์หมุนกลับ
    delay(15);               // รอให้เซอร์โวมอเตอร์หมุนเสร็จ
  }
} 