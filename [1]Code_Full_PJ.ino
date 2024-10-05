#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ThingSpeak.h>

#define TRIG_PIN D6
#define ECHO_PIN D8
#define GREEN_LED_PIN D2
#define YELLOW_LED_PIN D1
#define RED_LED_PIN D0

const char* ssid = "HUAWEI_2310";
const char* password = "1234567890";
const char* mqttServer = "mqtt.netpie.io";
const char* clientID = "a30e45c5-dc80-4ba7-a351-b98487039192";
const char* token = "9WTqzt3Q46rPVe1HrZg6LsDztwSh7hHL";
const char* secret = "g4t4R2MAcZAzk9yzGn8ACEwQB41XYcZi";

unsigned long myChannelNumber = 2655229;
const char* myWriteAPIKey = "DYM7MG2V8FLLSBRN";

WiFiClient espClient;
PubSubClient client(espClient);

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

// ฟังก์ชันเชื่อมต่อ MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID, token, secret)) {
      Serial.println("connected");
      client.subscribe("myTopic");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
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

// ฟังก์ชันวัดระยะและส่งข้อมูลไปยัง MQTT
void measure_and_publish_distance() {
  long distance = measure_distance();  
  if (distance == -1) {
    Serial.println("Failed to measure distance.");
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  String distanceStr = String(distance);
  client.publish("sensor/distance", distanceStr.c_str());
  control_led(distance);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  reconnect();
  ThingSpeak.begin(espClient);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  measure_and_publish_distance();
  delay(1000);
}

// ฟังก์ชัน callback สำหรับ MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  // จัดการข้อความจาก MQTT (ยังไม่ใช้งาน)
} 
