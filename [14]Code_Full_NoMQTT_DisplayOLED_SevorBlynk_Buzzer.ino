// Define Blynk template ID and name at the very top
#define BLYNK_TEMPLATE_ID "TMPL65HBVGn3o"
#define BLYNK_TEMPLATE_NAME "HC04AndSevor"
#define BLYNK_AUTH_TOKEN "ue2aKWfmhmBe02cDLVOby5JJyw7_19xJ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>  // ไลบรารีสำหรับเซอร์โวมอเตอร์
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ThingSpeak.h>

// Wi-Fi credentials
const char* ssid = "HUAWEI_2310"; // Wi-Fi name
const char* password = "1234567890"; // Wi-Fi password

// ThingSpeak credentials
unsigned long myChannelNumber = 2655229;
const char* myWriteAPIKey = "DYM7MG2V8FLLSBRN";

WiFiClient espClient;
int lastLedState = -1;

// Servo setup
Servo myServo;  // สร้างอ็อบเจกต์เซอร์โว
int servoPin = D4;  // กำหนดพอร์ต D4 (GPIO2) สำหรับเซอร์โวมอเตอร์
int angle = 0;  // มุมเริ่มต้นของเซอร์โวมอเตอร์
int speed = 15;  // กำหนดความเร็วในการหมุน (ยิ่งค่าน้อย ยิ่งเร็ว)

// OLED setup
#define SCREEN_WIDTH 128  // ความกว้างของหน้าจอ OLED
#define SCREEN_HEIGHT 64  // ความสูงของหน้าจอ OLED
#define OLED_RESET -1     // ไม่มีขาสำหรับรีเซ็ต
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Ultrasonic sensor and LED pins
#define TRIG_PIN D6
#define ECHO_PIN D8
#define GREEN_LED_PIN D2
#define YELLOW_LED_PIN D1
#define RED_LED_PIN D0

// Buzzer pin
#define BUZZER_PIN D3  // กำหนดพอร์ต D3 สำหรับ Buzzer

// ฟังก์ชันควบคุมการหมุนเซอร์โวมอเตอร์ตามค่าที่ได้จาก Blynk
void control_servo(int target_angle) {
  if (angle < target_angle) {
    for (int pos = angle; pos <= target_angle; pos += 1) { 
      myServo.write(pos);
      delay(speed);  // กำหนดความเร็วในการหมุน
    }
  } else if (angle > target_angle) {
    for (int pos = angle; pos >= target_angle; pos -= 1) {
      myServo.write(pos);
      delay(speed);  // กำหนดความเร็วในการหมุน
    }
  }
  angle = target_angle;  // บันทึกมุมปัจจุบันเพื่อใช้เป็นค่าเริ่มต้นครั้งต่อไป
}

// ฟังก์ชันเชื่อมต่อ Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

// ฟังก์ชันวัดระยะทางด้วยเซ็นเซอร์อัลตราโซนิก
long measure_distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  if (duration == 0) {
    Serial.println("Warning: No pulse detected");
    return -1; // คืนค่าที่แสดงว่าการวัดล้มเหลว
  }

  long distance = duration * 0.034 / 2;

  if (distance > 700 || distance <= 0) { 
    Serial.println("Distance out of range");
    return -1; // คืนค่าที่แสดงว่าระยะทางอยู่นอกช่วงที่กำหนด
  }

  return distance;
}

// ฟังก์ชันส่งข้อมูลไปยัง ThingSpeak
void send_to_thingspeak(long distance, int ledStatus) {
  ThingSpeak.setField(1, distance);    
  ThingSpeak.setField(2, ledStatus);   
  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.print("Error sending data to ThingSpeak, HTTP code: ");
    Serial.println(httpCode);
  }
}

// ฟังก์ชันแสดงผลบน OLED
void display_distance_with_face(long distance, void (*drawFace)()) {
  display.clearDisplay();  
  drawFace();  // แสดงหน้าตามอารมณ์ที่มุมบนซ้าย
  display.setTextSize(1);  // ขนาดเล็กสำหรับมุมขวาบน
  display.setCursor(110, 0);  // กำหนดตำแหน่งที่มุมขวาบน
  display.print("cm");

  display.setTextSize(2);              
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 50);  // ปรับให้ข้อความอยู่ด้านล่างของหน้าจอ
  display.print("Dist: ");
  display.print(distance);             
  display.display();
}

// ฟังก์ชันควบคุม LED และอารมณ์บนหน้าจอ OLED ตามระยะทาง
void control_led(long distance) {
  int ledState = 0;

  if (distance < 10) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = 2;
    display_distance_with_face(distance, drawSurprisedFace); // แสดงหน้าตกใจ
    
    // ส่งเสียงเตือนจาก Buzzer
    digitalWrite(BUZZER_PIN, HIGH); // เปิด Buzzer
    delay(500); // ส่งเสียง 500 ms
    digitalWrite(BUZZER_PIN, LOW);  // ปิด Buzzer
    delay(500); // ส่งเสียง 500 ms
    // ส่งเสียงเตือนจาก Buzzer
    digitalWrite(BUZZER_PIN, HIGH); // เปิด Buzzer
    delay(500); // ส่งเสียง 500 ms
    digitalWrite(BUZZER_PIN, LOW);  // ปิด Buzzer
    delay(500); // ส่งเสียง 500 ms
    // ส่งเสียงเตือนจาก Buzzer
    digitalWrite(BUZZER_PIN, HIGH); // เปิด Buzzer
    delay(500); // ส่งเสียง 500 ms
    digitalWrite(BUZZER_PIN, LOW);  // ปิด Buzzer

  } else if (distance < 20) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = 1;
    display_distance_with_face(distance, drawSuspiciousFace); // แสดงหน้าระแวง
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    ledState = 0;
    display_distance_with_face(distance, drawSmileyFace); // แสดงหน้ายิ้ม
  }

  if (ledState != lastLedState) {
    send_to_thingspeak(distance, ledState); 
    lastLedState = ledState;
  }
}

// ฟังก์ชันวาดหน้าต่างๆ บน OLED
void drawSmileyFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);  
  display.fillCircle(16, 16, 2, SSD1306_WHITE);  
  display.fillCircle(24, 16, 2, SSD1306_WHITE);  
  display.drawLine(16, 24, 24, 24, SSD1306_WHITE); 
}

void drawSuspiciousFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);  
  display.drawRect(12, 14, 4, 2, SSD1306_WHITE);  
  display.drawRect(20, 14, 4, 2, SSD1306_WHITE);  
  display.drawLine(16, 24, 24, 24, SSD1306_WHITE); 
}

void drawSurprisedFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);  
  display.drawCircle(16, 16, 2, SSD1306_WHITE);  
  display.drawCircle(24, 16, 2, SSD1306_WHITE);  
  display.drawCircle(20, 24, 3, SSD1306_WHITE);  
}

// Blynk V0: Servo control
BLYNK_WRITE(V0) {
  int target_angle = param.asInt();  // อ่านค่ามุมจากแอป Blynk
  Serial.println(target_angle);  // พิมพ์มุมที่ได้รับออกที่ Serial Monitor
  control_servo(target_angle);  // เรียกฟังก์ชันเพื่อควบคุมเซอร์โวมอเตอร์
}

// Blynk V1: Servo speed control
BLYNK_WRITE(V1) {
  speed = param.asInt();  // อ่านค่าความเร็วจากแอป Blynk
  Serial.print("Speed set to: ");
  Serial.println(speed);  // พิมพ์ความเร็วที่ได้รับออกที่ Serial Monitor
}

// ฟังก์ชันตั้งค่าเริ่มต้น
void setup() {
  Serial.begin(115200);
  setup_wifi();
  ThingSpeak.begin(espClient);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);  // เริ่มการเชื่อมต่อกับ Blynk
  myServo.attach(servoPin);  // เชื่อมต่อเซอร์โวมอเตอร์ที่พอร์ต D4
  myServo.write(angle);  // ตั้งค่าเริ่มต้นของเซอร์โวมอเตอร์

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);  // ตั้งค่า D3 เป็น OUTPUT สำหรับ Buzzer

  Wire.begin(D5, D7);  // เปลี่ยนไปใช้ D5 เป็น SCL และ D7 เป็น SDA
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.display();
  delay(2000);  
  display.clearDisplay();  
}

// ฟังก์ชันทำงานซ้ำในลูป
void loop() {
  long distance = measure_distance();
  
  if (distance == -1) {
    Serial.println("Failed to measure distance, skipping...");
    return; // ข้ามการดำเนินการหากการวัดล้มเหลว
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  control_led(distance);
  Blynk.run();
  delay(1000);
}
