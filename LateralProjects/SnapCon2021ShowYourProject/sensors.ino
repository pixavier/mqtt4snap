#include <ESP8266WiFi.h>
#include <ESP.h>
#include <PubSubClient.h>
#include <SimpleTimer.h> 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


///////////////////////////////////////

char ssid0[50] = "ssid"; 
char pass0[50] = "psswd";

///////////////////////////////////////

char mqtt_user[50]   = "";
char mqtt_pass[50]   = "";
char mqtt_broker[50] = "test.mosquitto.org";
char mqtt_port[50]   = "1883";

String stMac;
char mac[50];
String ip;
char message_buff[100];
char topic_buff[100];
char payload_buff[100];

SimpleTimer timer;
WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_MPU6050 mpu;

/****************************************************************************************/
void setup() {
  delay(100);
  Serial.begin(74880);
  setupWifi();
  int port = atoi(mqtt_port);
  client.setServer(mqtt_broker, port); 

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
}

/****************************************************************************************/
void loop() {
  timer.run();
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Inclination X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" Celsius");

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);

  char msg[128];
  char msg2[128];
  sprintf(msg, "{\"x\":%f, \"y\":%f, \"z\":%f}", a.acceleration.x, a.acceleration.y, a.acceleration.z);
  client.publish("xpi/sensors/pos", msg); 
  delay(200);
  sprintf(msg2, "{\"temp\":%f, \"light\":%f}", temp.temperature, voltage);
  client.publish("xpi/sensors/ambient", msg2); 
  delay(200);
}

/****************************************************************************************/
void setupWifi() {
    delay(10);
    Serial.println("");
    Serial.println("Connecting to 0 ");
    WiFi.begin(ssid0, pass0);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) {
      delay(500);
      Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    ip = WiFi.localIP().toString();
    Serial.println(ip);
    Serial.println(WiFi.macAddress());
    stMac = WiFi.macAddress();
    stMac.replace(":", "_");
    Serial.println(stMac);
}


/****************************************************************************************/
void connectMqtt() {
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    stMac.toCharArray(mac, 49);
    if (client.connect(mac, mqtt_user, mqtt_pass)) { 
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
