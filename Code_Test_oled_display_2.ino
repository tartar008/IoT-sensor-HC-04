#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // ความกว้างของหน้าจอ OLED
#define SCREEN_HEIGHT 64  // ความสูงของหน้าจอ OLED
#define OLED_RESET -1     // ไม่มีขาสำหรับรีเซ็ต
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int counter = 0; // ตัวแปรนับเลข

void setup() {
  Serial.begin(115200);
  
  // เริ่มต้นการสื่อสารกับจอ OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED ไม่สามารถทำงานได้"));
    for (;;);
  }
  display.clearDisplay();  // ลบข้อมูลบนหน้าจอ

  // วาดหน้าตาเริ่มต้น (หน้ายิ้ม)
  drawSmileyFace();
}

void loop() {
  delay(5000); // หน่วงเวลา 5 วินาที
  drawSuspiciousFace(); // เปลี่ยนเป็นหน้าระแวง
  updateCounter(); // แสดงเลขที่เพิ่มขึ้น
  
  delay(5000); // หน่วงเวลา 5 วินาที
  drawSurprisedFace(); // เปลี่ยนเป็นหน้าตกใจ
  updateCounter(); // แสดงเลขที่เพิ่มขึ้น
  
  delay(5000); // หน่วงเวลา 5 วินาที
  drawSmileyFace(); // เปลี่ยนกลับเป็นหน้ายิ้ม
  updateCounter(); // แสดงเลขที่เพิ่มขึ้น
}

void drawSmileyFace() {
  display.clearDisplay();
  
  // วาดใบหน้า (วงกลม)
  display.drawCircle(32, 32, 20, SSD1306_WHITE);  // วงกลมที่กลางด้านซ้าย (32, 32) และรัศมี 20
  
  // วาดตาซ้าย
  display.fillCircle(22, 24, 2, SSD1306_WHITE);  // วาดวงกลมเต็ม (ตาซ้าย)
  
  // วาดตาขวา
  display.fillCircle(42, 24, 2, SSD1306_WHITE);  // วาดวงกลมเต็ม (ตาขวา)
  
  // วาดปากยิ้ม
  display.drawLine(22, 40, 42, 40, SSD1306_WHITE); // เส้นตรงล่างของปาก
  display.drawLine(22, 40, 18, 36, SSD1306_WHITE); // เส้นเฉียงซ้าย
  display.drawLine(42, 40, 46, 36, SSD1306_WHITE); // เส้นเฉียงขวา
  
  display.display();  // แสดงผลบนหน้าจอ
}

void drawSuspiciousFace() {
  display.clearDisplay();
  
  // วาดใบหน้า (วงกลม)
  display.drawCircle(32, 32, 20, SSD1306_WHITE);  // วงกลมที่กลางด้านซ้าย (32, 32) และรัศมี 20
  
  // วาดตาซ้ายแบบระแวง
  display.drawRect(18, 22, 8, 4, SSD1306_WHITE);  // วาดสี่เหลี่ยม (ตาซ้าย)
  
  // วาดตาขวาแบบระแวง
  display.drawRect(38, 22, 8, 4, SSD1306_WHITE);  // วาดสี่เหลี่ยม (ตาขวา)
  
  // วาดปากแบบเส้นตรง
  display.drawLine(22, 40, 42, 40, SSD1306_WHITE); // ปากเป็นเส้นตรง
  
  display.display();  // แสดงผลบนหน้าจอ
}

void drawSurprisedFace() {
  display.clearDisplay();
  
  // วาดใบหน้า (วงกลม)
  display.drawCircle(32, 32, 20, SSD1306_WHITE);  // วงกลมที่กลางด้านซ้าย (32, 32) และรัศมี 20
  
  // วาดตาซ้ายแบบตกใจ
  display.drawCircle(22, 24, 3, SSD1306_WHITE);  // วาดวงกลมว่าง (ตาซ้าย)
  
  // วาดตาขวาแบบตกใจ
  display.drawCircle(42, 24, 3, SSD1306_WHITE);  // วาดวงกลมว่าง (ตาขวา)
  
  // วาดปากวงกลมแบบตกใจ
  display.drawCircle(32, 40, 5, SSD1306_WHITE);  // ปากเป็นวงกลม
  
  display.display();  // แสดงผลบนหน้าจอ
}

void updateCounter() {
  // ลบเฉพาะส่วนที่จะแสดงเลข (ด้านขวาของหน้าจอ)
  display.fillRect(80, 0, 48, 64, SSD1306_BLACK); // ลบพื้นที่ด้านขวาสำหรับแสดงตัวเลข
  
  display.setTextSize(2);        // ตั้งขนาดตัวอักษรให้ใหญ่ขึ้น
  display.setTextColor(SSD1306_WHITE);  // สีตัวอักษรเป็นสีขาว
  display.setCursor(90, 28);     // ตำแหน่งในการแสดงเลข (ด้านขวา)
  display.print(counter);        // แสดงเลข
  
  display.display();  // แสดงผลบนหน้าจอ
  
  counter++; // เพิ่มค่า counter ทีละ 1
}
