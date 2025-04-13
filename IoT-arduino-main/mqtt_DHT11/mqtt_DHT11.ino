/*
 nodemcu D1, D2
 DHT11 온습도 센서 측정해서 mqttbroker로 pub 해보는 예제
 스마트폰 앱을 이용해 sub 하여 온도 습도 값을 스마트폰으로 받아 확인하는 예제 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 5 // GPIO D1 핀 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "306";
const char* password = "14239823";
const char* mqtt_server = "192.168.0.28"; //raspberrypi4 server 1 IP
//const char* mqtt_username = "hsb";   //homeassistant 이용시 username 필요
//const char* mqtt_password = "1423";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  IPAddress ip (192, 168, 0 , 112);    //고정 IP 부여 
  IPAddress gateway(192, 168, 0 ,1);   //게이트웨이 설정 (공유기 설정에서 알 수 있음)
  IPAddress subnet(255,255,255,0);     //subnet 네트워크 (공유기 설정에서 알 수 있음)
  WiFi.config(ip, gateway, subnet);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {  //수신부 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
//----------------------------------------------------------------------------------------
  // test용 내장 LED
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  
  } else {
    digitalWrite(BUILTIN_LED, HIGH); 
  }
}
void reconnect() {  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "DHT11-";
    clientId += String("nodeumcu2");  //장비 이름 
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world"); //test용 토픽 
      client.subscribe("node2BuiltInLED");  //어떤 토픽을 구독 하겠냐 구독 할 토픽    

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {  //항상 연결 상태 유지 
    reconnect();
  }
  client.loop();

  unsigned long now = millis();  //딜레이 사용 금지 millis 사용 딜레이 사용시 서버 끊김 
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "nodemcu2보드 연결 #%ld초 경과", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("hsb/node2connection", msg); //발행 부분 어플리케이션에서는 outhsb를 구독을 해야함 
//DHT sensor part 
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
//측정값 출력
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.println(t);
    snprintf (msg, MSG_BUFFER_SIZE, "%.1f°C", t);
    client.publish("hsb/temp", msg);
    snprintf (msg, MSG_BUFFER_SIZE, "%.1f%%", h);
    client.publish("hsb/humi", msg);
  }
}
