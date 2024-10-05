#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// กำหนดขาที่ใช้เชื่อมต่อกับเซ็นเซอร์และ LED
#define TRIG_PIN D6
#define ECHO_PIN D8
#define GREEN_LED_PIN D2
#define YELLOW_LED_PIN D1
#define RED_LED_PIN D0

// กำหนดขา SCL และ SDA ที่ใช้กับหน้าจอ OLED
#define SCREEN_WIDTH 128  // ความกว้างของหน้าจอ OLED
#define SCREEN_HEIGHT 64  // ความสูงของหน้าจอ OLED
#define OLED_RESET -1     // ไม่มีขาสำหรับรีเซ็ต
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// กำหนดการเชื่อมต่อ Wi-Fi และ ThingSpeak
const char* ssid = "HUAWEI_2310";
const char* password = "1234567890";
unsigned long myChannelNumber = 2655229;
const char* myWriteAPIKey = "DYM7MG2V8FLLSBRN";

WiFiClient espClient;
int lastLedState = -1;

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

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // จำกัดเวลาการตอบสนองที่ 30 ms

  // ตรวจสอบว่าไม่มีพัลส์หรือเกินขอบเขต
  if (duration == 0) {
    return 200;  // กำหนดให้แสดงระยะทางสูงสุดที่ 200 cm หากไม่มีพัลส์
  }

  long distance = duration * 0.034 / 2; 

  // กำหนดขอบเขตระยะทางสูงสุดที่ 200 cm
  if (distance > 200) {
    distance = 200;  // ถ้าเกิน 200 cm ให้แสดงระยะทางที่ 200 cm
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

// ฟังก์ชันแสดงผลบน OLED ปรับให้แสดงอารมณ์และระยะทาง
void display_distance_with_face(long distance, void (*drawFace)()) {
  display.clearDisplay();  
  drawFace();  // แสดงหน้าตามอารมณ์ที่มุมบนซ้าย
  
  // แสดงคำว่า "cm" ที่มุมขวาบน
  display.setTextSize(1);  // ขนาดเล็กสำหรับมุมขวาบน
  display.setCursor(110, 0);  // กำหนดตำแหน่งที่มุมขวาบน
  display.print("cm");

  // แสดงข้อความระยะทางที่ด้านล่างทั้งหมด
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

// ฟังก์ชันวาดหน้าต่างๆ บน OLED ปรับให้แสดงในมุมบนซ้าย
void drawSmileyFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);  // วาดหน้าที่มุมบนซ้าย
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

// ฟังก์ชันตั้งค่าเริ่มต้น
void setup() {
  Serial.begin(115200);
  setup_wifi();
  ThingSpeak.begin(espClient);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

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
  // if (distance == -1) {
  //   Serial.println("Failed to measure distance.");
  //   return;
  // }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  control_led(distance);
  delay(1000);
}
