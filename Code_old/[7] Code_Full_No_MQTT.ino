#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN D6
#define ECHO_PIN D8
#define GREEN_LED_PIN D2
#define YELLOW_LED_PIN D1
#define RED_LED_PIN D0

#define SCREEN_WIDTH 128 // ความกว้างหน้าจอ OLED
#define SCREEN_HEIGHT 64  // ความสูงหน้าจอ OLED
#define OLED_RESET -1  // ไม่มีขา reset
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 

  if (duration == 0) {
    Serial.println("Warning: No pulse detected or out of range");
    return -1;
  }

  long distance = duration * 0.034 / 2; 
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

// ฟังก์ชันควบคุม LED ตามระยะทาง
void control_led(long distance) {
  int ledState = 0;

  if (distance < 10) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = 2;
  } else if (distance < 20) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = 1;
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    ledState = 0;
  }

  if (ledState != lastLedState) {
    send_to_thingspeak(distance, ledState); 
    lastLedState = ledState;
  }
}

// ฟังก์ชันแสดงผลบน OLED
void display_distance(long distance) {
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);             
  display.print("Distance:");
  display.setCursor(0, 30);            
  display.print(distance);             
  display.print(" cm");
  display.display();                   
}

// ฟังก์ชันวัดระยะและส่งข้อมูลไปยัง ThingSpeak
void measure_and_publish_distance() {
  long distance = measure_distance();  
  if (distance == -1) {
    Serial.println("Failed to measure distance.");
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  control_led(distance);
  display_distance(distance); // แสดงระยะบน OLED
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  ThingSpeak.begin(espClient);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // เริ่มต้นการทำงานของ OLED
  Wire.begin(D3, D4); // ใช้ D3 เป็น SDA และ D4 เป็น SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.display();
  delay(2000);  
  display.clearDisplay();  
}

void loop() {
  measure_and_publish_distance();
  delay(1000);
}
