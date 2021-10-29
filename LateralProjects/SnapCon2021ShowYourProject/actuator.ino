  

#include <ESP8266WiFi.h>
#include <ESP.h>
#include <PubSubClient.h>
#include <SimpleTimer.h> 


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
const byte relayPin = 0;   // ESP-01
//const byte relayPin = 16;     // NodeMCU
char message_buff[100];
char topic_buff[100];
char payload_buff[100];

SimpleTimer timer;
WiFiClient espClient;
PubSubClient client(espClient);

char callback_topic[100];


/****************************************************************************************/
void setup() {
  pinMode(relayPin, OUTPUT);
  delay(100);
  Serial.begin(74880);
  setupWifi();
  int port = atoi(mqtt_port);
  client.setServer(mqtt_broker, port); 
  client.setCallback(callback);
}

/****************************************************************************************/
void loop() {
  timer.run();
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();
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
void callback(char *topic_buff, byte *payload, unsigned int length) {
  String topic = String(topic_buff);
  
  Serial.print("Payload length: ");
  Serial.println(length);

  int i;
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  String msgString = String(message_buff);

  Serial.print("Val: ");
  Serial.println(msgString);
  Serial.println("Got topic " + topic);

  if (topic.indexOf("xpi/actuator") >= 0) {
      int time = msgString.toInt();
      if (time > 5000) {
        time = 5000;
      }
      openRelay(time);
  }
}

/****************************************************************************************/
void connectMqtt() {
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    stMac.toCharArray(mac, 49);
    if (client.connect(mac, mqtt_user, mqtt_pass)) { 
      Serial.println("connected");
      String topic = "xpi/actuator";
      topic.toCharArray(topic_buff, 99);
      client.subscribe(topic_buff);
      Serial.print("subscribed topic: ");
      Serial.println(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

/****************************************************************************************/
void openRelay(int time) {
  digitalWrite(relayPin, HIGH);
  Serial.println("ON");
  timer.setTimeout(time, openRelayCallback);
}

/****************************************************************************************/
void openRelayCallback(){
  digitalWrite(relayPin, LOW);
  Serial.println("OFF");
}
