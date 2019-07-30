#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include "ArduinoJson.h"
#include <SoftwareSerial.h>
const byte RX = D1;
const byte TX = D2;
#include <SHT1x.h>

#define mqtt_server "broker.hivemq.com"
//#define mqtt_server "192.168.0.197"
#define mqtt_topic_pub "server_room/sht10"
#define mqtt_topic_sub "zone_3/box_1/led"
#define dataPin  D4
#define clockPin D3
SHT1x sht1x(dataPin, clockPin);


const char *ssid =  "p2808";     /// replace with your wifi ssid and wpa2 key./
const char *pass =  "20112010";


WiFiClient ESPclient;
PubSubClient client(ESPclient);

long lastMsg = 0;
char msg[50];
int value = 0;


//unsigned long time = 0;
void setup_wifi();

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Ham callback de nhan du lieu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");


  Serial.println();
  delay(100);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(mqtt_topic_pub, "hello world");
      // ... and resubscribe
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Nhan du lieu tu arduino va publish/subscribe len server
StaticJsonBuffer<200> jsonBuffer;
JsonObject& json_buffer = jsonBuffer.createObject();
char buffer_[200];

void loop()
{
  // Kiem tra ket noi
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  
  float temp_c;
  float temp_f;
  float humidity;

  // Read values from the sensor
  temp_c = sht1x.readTemperatureC();
  temp_f = sht1x.readTemperatureF();
  humidity = sht1x.readHumidity();

  if (temp_c<0 || temp_c>100 || humidity<0 || humidity>100){
    
  }
  else {
    // Print the values to the serial port
    Serial.print("Temperature: ");
    Serial.print(temp_c);//, DEC);
    Serial.print("C / ");
    Serial.print(temp_f);//, DEC);
    Serial.print("F. Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  
    json_buffer["temperature"] = temp_c;
    json_buffer["humidity"] = humidity;
    
    json_buffer.printTo(buffer_, sizeof(buffer_));
    Serial.println(buffer_);
    client.publish(mqtt_topic_pub, buffer_);
  
    delay(2000);
  }
}

