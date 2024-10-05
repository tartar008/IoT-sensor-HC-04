#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128  // ความกว้างของหน้าจอ OLED
#define SCREEN_HEIGHT 64  // ความสูงของหน้าจอ OLED
#define OLED_RESET -1     // ไม่มีขาสำหรับรีเซ็ต
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // เริ่มต้นการสื่อสารกับจอ OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED ไม่สามารถทำงานได้"));
    for(;;);
  }
  display.clearDisplay(); // ลบข้อมูลบนหน้าจอ
  display.setTextSize(2); // ขนาดตัวอักษร
  display.setTextColor(SSD1306_WHITE); // สีของข้อความ (ขาว)
  display.setCursor(0,10); // กำหนดตำแหน่งข้อความที่จะแสดง
  display.display();
}

void loop() {
  display.clearDisplay(); // ลบข้อมูลเก่าออกจากหน้าจอ

  // สุ่มตัวเลขระหว่าง 0 ถึง 999
  int randomNumber = random(0, 1000);

  // แสดงตัวเลขสุ่มบนหน้าจอ
  display.setCursor(0, 10);
  display.print("Random: ");
  display.print(randomNumber);

  display.display(); // อัปเดตหน้าจอแสดงผล

  delay(1000); // หน่วงเวลา 1 วินาที
}
