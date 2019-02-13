#include<stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// PIN D1 on ESP -> GPIO5
#define ONE_WIRE_DATA_PIN 5

const char* ssid = "TELUS0061";
const char* wifi_password = "4ismjd62yr";
const char* mqtt_server_address = "192.168.1.69";
const int mqtt_server_port = 1883;

char buff[10];
unsigned long intervalPublish = 10000; // Request NTP time every 10 seconds
unsigned long prevPublishTime = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

OneWire ourWire(ONE_WIRE_DATA_PIN);
DallasTemperature sensors(&ourWire);

void setup_wifi()
{
  //wifiMulti.addAP("TELUS0061", "4ismjd62yr");   // add Wi-Fi networks you want to connect to
  //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  WiFi.begin(ssid, wifi_password);

  Serial.print("Connecting to WiFi network ");
  Serial.print(WiFi.SSID());              // Tell us what network we're connected to
  Serial.println("...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Messageved [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    Serial.println("Turning LED ON");
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    Serial.println("Turning LED OFF");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT broker ");
    Serial.print(mqtt_server_address);
    Serial.print(":");
    Serial.print(mqtt_server_port);
    Serial.println("...");
    // Attempt to connect
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker!");
      // Once connected, publish an announcement...
      mqttClient.publish("/home/1/esp01/p1/state", "hello world");
      // ... and resubscribe
      mqttClient.subscribe("/home/1/esp01/p1/command");
    } else {
      Serial.print("Failed");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_led()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  setup_led();
  
  setup_wifi();
  mqttClient.setServer(mqtt_server_address, mqtt_server_port);
  mqttClient.setCallback(callback);

  sensors.begin();
}

void loop()
{
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  sensors.requestTemperatures(); //Prepara el sensor para la lectura
  unsigned long currentMillis = millis();
  if (currentMillis - prevPublishTime > intervalPublish) {
    prevPublishTime = currentMillis;
    float tempC = sensors.getTempCByIndex(0);
    Serial.print("TEMP: ");
    Serial.println(tempC);
    dtostrf(tempC, 4, 6, buff);
    mqttClient.publish("/home/1/esp01/p1/state", buff);
  }
}
