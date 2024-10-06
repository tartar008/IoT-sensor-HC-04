// Define Blynk template ID and name at the very top
#define BLYNK_TEMPLATE_ID "TMPL65HBVGn3o"
#define BLYNK_TEMPLATE_NAME "HC04AndSevor"
#define BLYNK_AUTH_TOKEN "ue2aKWfmhmBe02cDLVOby5JJyw7_19xJ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ThingSpeak.h>

// Wi-Fi credentials
const char* ssid = "HUAWEI_2310";
const char* password = "1234567890";

// ThingSpeak credentials
unsigned long myChannelNumber = 2655229;
const char* myWriteAPIKey = "DYM7MG2V8FLLSBRN";

WiFiClient espClient;
String lastLedState = ""; // Store last LED state as a string

// Servo setup
Servo myServo;
int servoPin = D4;
int angle = 0;
int speed = 15;

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Ultrasonic sensor and LED pins
#define TRIG_PIN D6
#define ECHO_PIN D8
#define GREEN_LED_PIN D2
#define YELLOW_LED_PIN D1
#define RED_LED_PIN D0

// Buzzer pin
#define BUZZER_PIN D3

// Function to control the servo motor based on the target angle
void control_servo(int target_angle) {
  if (angle < target_angle) {
    for (int pos = angle; pos <= target_angle; pos += 1) {
      myServo.write(pos);
      delay(speed);
    }
  } else if (angle > target_angle) {
    for (int pos = angle; pos >= target_angle; pos -= 1) {
      myServo.write(pos);
      delay(speed);
    }
  }
  angle = target_angle;
}

// Function to connect to Wi-Fi
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

// Function to measure distance using the ultrasonic sensor
long measure_distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  if (duration == 0) {
    Serial.println("Warning: No pulse detected");
    return -1; 
  }

  long distance = duration * 0.034 / 2;

  if (distance > 700 || distance <= 0) {
    Serial.println("Distance out of range");
    return -1;
  }

  return distance;
}

// Function to send data to ThingSpeak
void send_to_thingspeak(long distance, String ledStatus) {
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

// Function to display distance and faces on OLED
void display_distance_with_face(long distance, void (*drawFace)()) {
  display.clearDisplay();
  drawFace();
  display.setTextSize(1);
  display.setCursor(110, 0);
  display.print("cm");

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 50);
  display.print("Dist: ");
  display.print(distance);
  display.display();
}

// Function to control LEDs and OLED face based on distance
void control_led(long distance) {
  String ledState;

  if (distance < 10) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = "Red";
    display_distance_with_face(distance, drawSurprisedFace);

    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);

  } else if (distance < 20) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    ledState = "Yellow";
    display_distance_with_face(distance, drawSuspiciousFace);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    ledState = "Green";
    display_distance_with_face(distance, drawSmileyFace);
  }

  if (ledState != lastLedState) {
    send_to_thingspeak(distance, ledState);
    lastLedState = ledState;
  }
}

// Function to draw smiley face on OLED
void drawSmileyFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);
  display.fillCircle(16, 16, 2, SSD1306_WHITE);
  display.fillCircle(24, 16, 2, SSD1306_WHITE);
  display.drawLine(16, 24, 24, 24, SSD1306_WHITE);
}

// Function to draw suspicious face on OLED
void drawSuspiciousFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);
  display.drawRect(12, 14, 4, 2, SSD1306_WHITE);
  display.drawRect(20, 14, 4, 2, SSD1306_WHITE);
  display.drawLine(16, 24, 24, 24, SSD1306_WHITE);
}

// Function to draw surprised face on OLED
void drawSurprisedFace() {
  display.drawCircle(20, 20, 10, SSD1306_WHITE);
  display.drawCircle(16, 16, 2, SSD1306_WHITE);
  display.drawCircle(24, 16, 2, SSD1306_WHITE);
  display.drawCircle(20, 24, 3, SSD1306_WHITE);
}

// Blynk V0: Servo control
BLYNK_WRITE(V0) {
  int target_angle = param.asInt();
  Serial.println(target_angle);
  control_servo(target_angle);
}

// Blynk V1: Servo speed control
BLYNK_WRITE(V1) {
  speed = param.asInt();
  Serial.print("Speed set to: ");
  Serial.println(speed);
}

// Setup function
void setup() {
  Serial.begin(115200);
  setup_wifi();
  ThingSpeak.begin(espClient);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  myServo.attach(servoPin);
  myServo.write(angle);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(D5, D7);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

// Loop function
void loop() {
  long distance = measure_distance();

  if (distance == -1) {
    Serial.println("Failed to measure distance, skipping...");
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  control_led(distance);
  Blynk.run();
  delay(1000);
}
