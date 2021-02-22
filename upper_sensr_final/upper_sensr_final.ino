#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "DHT.h"
#include "String.h"
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <math.h>
#define DHTPIN 2
#define DHTTYPE DHT11
#define BH1750address 0x23
uint16_t val;//Binary datatype
byte buff[2];

DHT dht(DHTPIN, DHTTYPE);
// Update these with values suitable for your network.


const char* ssid = "OfficeConnect";
const char* password = "37280DE4BF";
const char* mqtt_server = "192.168.10.75";
String result; 
WiFiClient espClient;

PubSubClient client(espClient);

long lastMsg = 0;
int switches;
int led = 12;

char msg[50];

int value = 0;

 

void setup() {
  Wire.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();  
  pinMode(led, OUTPUT);
  
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

 

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");

  Serial.print(topic);

  Serial.print("] ");
  for (int i = 0; i < length; i++) {

    Serial.print((char)payload[i]);

  }
  Serial.println();
  Serial.print("payload = ");
  Serial.println(payload[0]);
switches = payload[0];
  Serial.print("switch = ");
  Serial.println(switches);
  Serial.println();
}
void reconnect() {

  // Loop until we're reconnected

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect

    if (client.connect("ESP8266Client")) {

      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("outTopic");
      Serial.print("Before:");
    Serial.println(client.subscribe("outTopic"));
    Serial.print("After:");
    Serial.println(client.subscribe("outTopic"));
    } else {

      Serial.print("failed, rc=");

      Serial.print(client.state());

      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying

      delay(2000);

    }

  }

}

void loop() { 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float sensorVoltage; 
  float sensorValue;
  Serial.print("switches : ");
Serial.println(switches);
  if(switches == 116){
    digitalWrite(led, HIGH);
  }else{
    digitalWrite(led, LOW);
  }
  BH1750_Init(BH1750address);
  delay(200);
  if(2==BH1750_Read(BH1750address)){
   val=((buff[0]<<8)|buff[1])/1.2;
    }
 
  Serial.print("Light: ");
  Serial.println(val); 
   sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*3.3;

  if(isnan(t) || isnan(h)){
    Serial.println("Failed to read DHT11");
  }else{
    Serial.print("Humidity");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature");
    Serial.print(t);
    Serial.println(" *C");
    Serial.print(sensorVoltage);
  Serial.println(" V");
  Serial.print(val);
  Serial.println(" light");
  }
  if (!client.connected()) {
    reconnect();}
  client.loop();
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
     result ="{\"temp\":"+String(t)+",\"hum\":"+String(h)+",\"gas\":"+String(sensorVoltage)+",\"light\":"+String(val)+"}";
client.publish("inTopic",result.c_str()); }

}

int BH1750_Read(int address){
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();  
  return i;
}
void BH1750_Init(int address){
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}
