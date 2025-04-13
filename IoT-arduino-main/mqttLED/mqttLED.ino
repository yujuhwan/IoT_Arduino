/*
 nodemcu D1번,D2,D5 핀으로 LED 제어 
*/

#include <ESP8266WiFi.h> // nodemcu 내부의 esp8266모듈을 사용
#include <PubSubClient.h> // MQTT 라이브러리 
#define LED1 5 // GPIO D1 핀 
#define LED2 4 // GPIO D2 핀 
#define LED3 14 // GPIO D5 핀
// Update these with values suitable for your network.

// 접속할 ssid와 비밀번호 설정 / 접속할 mqttsever ip 설정
const char* ssid = "306";
const char* password = "14239823";
const char* mqtt_server = "192.168.0.28"; //raspberry pi4 server 1 IP

WiFiClient wifiClient;
PubSubClient client(wifiClient);

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

  IPAddress ip (192, 168, 0 ,111);
  IPAddress gateway(192, 168, 0 ,1);
  IPAddress subnet(255,255,255,0);
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
  String topic2 = topic;

  if(topic2 == "hsb/room1"){//hsb집의 1번방의 조명 
    if((char)payload[0] == '0'){ // payload를 통해 0의 값을 받으면 
      digitalWrite(LED1, LOW); // LED1은 꺼진다.
    }
    else { //0 이외에 다른 값이 들어오면 
      digitalWrite(LED1, HIGH); //LED1을 켠다. 
    }
  }
    else if(topic2 == "hsb/room2") { // hsb집의 room2 라는 토픽이 들어온다면 
      if((char)payload[0] == '0') {  // 토픽의 payload를 통해 0의 값이 들어온다면 
        digitalWrite(LED2, LOW);   // LED2를 끈다. 
      }
      else{ //0이외의 값을 수신한다면, 
        digitalWrite(LED2, HIGH); // LED2를 켠다. 
      }
    }

    else if(topic2 == "hsb/room3") {
       if((char)payload[0] == '0') {  // 토픽의 payload를 통해 0의 값이 들어온다면 
         digitalWrite(LED3, LOW);   // LED3을 끈다. 
       }
       else{ //0이외의 값을 수신한다면, 
         digitalWrite(LED3, HIGH); // LED3을 켠다. 
       }
     }
     else if(topic2 == "node1BuiltInLED"){  // nodemcu 내장 LED 
        if((char)payload[0] == '1') {
          digitalWrite(BUILTIN_LED, LOW);
        }
        else {
          digitalWrite(BUILTIN_LED, HIGH);
        }
     }
     else if(topic2 == "hsb/roomall"){
      if((char)payload[0] == '0'){
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        digitalWrite(LED3, LOW);
        client.publish("hsb/room1", "0"); //room all 사용시 room1,2,3 의 상태가 같이 변화하도록 pub추가
        client.publish("hsb/room2", "0"); //즉, 버튼 3개의 상태를 pub한다. 
        client.publish("hsb/room3", "0");
      }
      else {
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED3, HIGH);
        client.publish("hsb/room1", "1"); //room all 사용시 room1,2,3 의 상태가 같이 변화하도록 pub추가
        client.publish("hsb/room2", "1"); //즉, 버튼 3개의 상태를 pub한다. 
        client.publish("hsb/room3", "1"); // 버튼이 켜졌다라는 1의 상태를 pub 한다. 
      } 
     }                                    // mqtt dashboard어플의 특성, sub에 토픽을 부여하고 pub에 토픽을 부여하지
     }                                    // 않으면, sub, pub 둘다 같은 토픽을 발행, 구독 한다. 
    
void reconnect() {  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "hsbmcu1";
    clientId += String("nodemcu1");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe("node1BuiltInLED");  // nodemcu1 BUILTIN_LED 구독 
      client.subscribe("hsb/room1");
      client.subscribe("hsb/room2");
      client.subscribe("hsb/room3"); 
      client.subscribe("hsb/roomall"); 
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
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
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
    snprintf (msg, MSG_BUFFER_SIZE, "nodemcu1 연결 #%ld초 경과", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("hsb1/node1connection", msg); //발행 부분 어플리케이션에서는 outhsb를 구독을 해야함 
    
  }
}
