/*
  XSwitch - Phần mềm điều khiển công tắc qua mobile
  Copyright (C) 2016 NHX
 
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.
  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "EEPROM.h"
#include <SHT1x.h>
#include "PubSubClient.h"
#include "ArduinoJson.h"



#define mqtt_topic_pub "server_room/sht10/pub"
#define mqtt_topic_sub "server_room/sht10/sub"
#define mqtt_server "broker.hivemq.com"
//#define mqtt_server "192.168.1.3"

// Specify data and clock connections and instantiate SHT1x object
#define dataPin  D4
#define clockPin D3
#define resetPin D5
SHT1x sht1x(dataPin, clockPin);
 
ESP8266WebServer server(80);
WiFiClient ESPclient;
PubSubClient client(ESPclient);

long lastMsg = 0;
char msg[50];
int value = 0;

const char*     ssid           = "XSwitch";
const char*     passphrase     = "000000000";
String          st;
String          content;
int             statusCode;




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

//  if (strcmp(topic, mqtt_topic_sub) == 0) {
//    payload[length] = '\0';  // Cắt bỏ dữ liệu thừa
//    char inData[80];
//    char payload_string[100];
//    strncpy(payload_string, (char*)payload, sizeof(payload_string)); // chuyển về dàng char
//    Serial.println(payload_string);
//    mySerial.print(payload_string);
//    client.publish(mqtt_topic_check_status, payload_string);
//  }
  Serial.println();
  delay(100);
}

void reconnect() {
  // Loop until we're reconnected
    for (int i=0; i<10; i++){
      if (!client.connected()){
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
          Serial.println("connected");
        } else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 1 seconds");
          // Wait 5 seconds before retrying
          delay(1000);
        }
      }
      else{
        break;
      }
    }
}



bool testWifi(void)
{
    int c = 0;
    Serial.println("Xin vui long doi ket noi WIFI");
    while ( c < 60 )
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        delay(1000);
        Serial.print(WiFi.status());
        c++;
    }
    Serial.println("");
    Serial.println("Thoi gian ket noi cham, Mo AP");
    return false;
}
 
void launchWeb(int webtype)
{
    Serial.println("");
    Serial.println("WiFi ket noi");
    Serial.print("Dia chi IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    createWebServer(webtype);
    // Start the server
    server.begin();
    Serial.println("May chu bat dau");
}
 
void setupAP(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    Serial.println("Tim hoan tat");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
            delay(10);
        }
    }
    Serial.println("");
    st = "<ol>";
    for (int i = 0; i < n; ++i)
    {
        // Print SSID and RSSI for each network found
        st += "<li>";
        st += WiFi.SSID(i);
        st += " (";
        st += WiFi.RSSI(i);
        st += ")";
        st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
        st += "</li>";
    }
    st += "</ol>";
    delay(100);
    Serial.println("softap");
    Serial.println(ssid);
    Serial.println(passphrase);
    WiFi.softAP(ssid, passphrase, 6);
 
    launchWeb(1);
    Serial.println("over");
}
 
void createWebServer(int webtype)
{
    if ( webtype == 1 )
    {
        server.on("/", []()
        {
            IPAddress ip = WiFi.softAPIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            content = "<!DOCTYPE HTML>\r\n<html><h2>XSwitch</h2>";
            //content += ipStr;
            //content += "<form method='get' action='setting'><table width='100%' border='1'><tr><td width=\"30%\"><label>Wifi</label></td><td width=\"70%\><input name='ssid' length=32 width='500'></td></tr><tr><td><label>Password</label></td><td><input name='pass' length=64 width='500'></td></tr><tr><td><label>Blynk</label></td><td><input name='blynk' length=32 width='500'></td></tr><tr><td></td><td><input type='submit'></tr></tr></table></form>";
            content += "<form method=\"get\" action=\"setting\">";
            content += "<div>Wifi</div>";
            content += "<div><input name=\"ssid\" size=\"40\"></div>";
            content += "<div>Mat Khau</div>";
            content += "<div><input name=\"pass\" size=\"40\"></div>";
//            content += "<div>MQTT Server</div>";
//            content += "<div><input name=\"mqtt_server\" size=\"40\"></div>";
            content += "<div><input type='submit'></div>";
 
            content += "<p>";
            content += st;
            content += "</p>";
            content += "</html>";
            server.send(200, "text/html", content);
        });
        server.on("/setting", []()
        {
            String qsid = server.arg("ssid");
            String qpass = server.arg("pass");
//            String qmqtt_server = server.arg("mqtt_server");
            if (qsid.length() > 0 && qpass.length() > 0)
            {
                EEPROM.begin(512);
                Serial.println("clearing eeprom");
                for (int i = 0; i < 128; ++i)
                {
                    EEPROM.write(i, 0);
                }
                EEPROM.commit();
                Serial.println(qsid);
                Serial.println("");
                Serial.println(qpass);
                Serial.println("");
//                Serial.println(qmqtt_server);
//                Serial.println("");
 
                Serial.println("writing eeprom ssid:");
                for (int i = 0; i < qsid.length(); ++i)
                {
                    EEPROM.write(i, qsid[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qsid[i]);
                }
 
                Serial.println("writing eeprom pass:");
                for (int i = 0; i < qpass.length(); ++i)
                {
                    EEPROM.write(32 + i, qpass[i]);
                    Serial.print("Wrote: ");
                    Serial.println(qpass[i]);
                }
 
//                Serial.println("writing eeprom mqtt_server:");
//                for (int i = 0; i < qmqtt_server.length(); ++i)
//                {
//                    EEPROM.write(96 + i, qmqtt_server[i]);
//                    Serial.print("Wrote: ");
//                    Serial.println(qmqtt_server[i]);
//                }
                EEPROM.commit();
                EEPROM.end();
                content = "{\"Success\":\"Luu vao he thong. Khoi dong lai ten wifi moi\"}";
                statusCode = 200;
            }
            else
            {
                content = "{\"Error\":\"404 not found\"}";
                statusCode = 404;
                Serial.println("Sending 404");
            }
            server.send(statusCode, "application/json", content);
        });
    }
    else if (webtype == 0)
    {
        server.on("/", []()
        {
            IPAddress ip = WiFi.localIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
        });
        server.on("/cleareeprom", []()
        {
            content = "<!DOCTYPE HTML>\r\n<html>";
            content += "<h2>XSwitch</h2><p>Clearing the EEPROM</p></html>";
            server.send(200, "text/html", content);
            Serial.println("clearing eeprom");
            for (int i = 0; i < 128; ++i)
            {
                EEPROM.write(i, 0);
            }
            EEPROM.commit();
        });
    }
}

String esid;
String epass = "";

void setup()
{
    Serial.begin(115200);
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    EEPROM.begin(512);
    delay(10);
    Serial.println("Startup");
    // read eeprom for ssid, pass and blynk
    Serial.println("Reading EEPROM ssid");
    for (int i = 0; i < 32; ++i)
    {
        esid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(esid.c_str());
    esid.trim();
 
    Serial.println("Reading EEPROM pass");
    for (int i = 32; i < 96; ++i)
    {
        epass += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(epass.c_str());
    epass.trim();
 
//    Serial.println("Reading EEPROM mqtt_server");
//    String mqtt_server = "";
//    for (int i = 96; i < 128; ++i)
//    {
//        mqtt_server += char(EEPROM.read(i));
//    }
//    Serial.print("mqtt_server: ");
//    Serial.println(mqtt_server.c_str());
//    mqtt_server.trim();
 
    if ( esid.length() > 1 )
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(esid.c_str(), epass.c_str());
        if (testWifi())
        {
//            launchWeb(0);
//            WiFi.disconnect();
            EEPROM.end();

            Serial.println("true");
            Serial.println(WiFi.localIP());

            return;
        }
    }
    setupAP();
    EEPROM.end();
    
}

StaticJsonBuffer<200> jsonBuffer;
JsonObject& json_buffer = jsonBuffer.createObject();
char buffer_[200];
int before = 0;
int after = 0;


void loop()
{   
//    Serial.println(esid.c_str());
//    Serial.println(epass.c_str());
  
    while (WiFi.status() != WL_CONNECTED)
    {
//      WiFi.reconnect();
      Serial.println("Try to reconnect to wifi after 5s!");
      WiFi.disconnect();
      WiFi.mode(WIFI_STA);
//      WiFi.begin("Bphone_3787", "bphone5239");
      WiFi.begin(esid.c_str(), epass.c_str());
      
      delay(5000);
//      Serial.print(".");
    }
    
//    Serial.print(after-before);
//    Serial.print();




    // Reset after 12 hours
//    if (after-before > 12*60*60*1000){
////    if (after-before>1000*5){
////        digitalWrite(resetPin, HIGH);
//        Serial.println("Reset!");
//        digitalWrite(resetPin, LOW);
//        before = after;
//    }

    after = millis();




    
    server.handleClient();
    
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
  
    // Print the values to the serial port
    Serial.print("Temperature: ");
    Serial.print(temp_c);//, DEC);
    Serial.print("C / ");
    Serial.print(temp_f);//, DEC);
    Serial.print("F. Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    if (temp_c<0 || temp_c>100 || humidity<0 || humidity>100){
    
    }
    else{
      json_buffer["temperature"] = temp_c;
      json_buffer["humidity"] = humidity;
      
      json_buffer.printTo(buffer_, sizeof(buffer_));
      Serial.println(buffer_);
      client.publish(mqtt_topic_pub, buffer_);
    }
    
    
    delay(10000);
}
