#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// WiFi Settings

const char* ssid = "YourSSID";
const char* password = "YourPassword";

// MQTT Settings

const char* mqtt_server = "YourBroker";
const char* mqtt_command_topic = "obisocket/0/command";
const char* mqtt_status_topic = "obisocket/0/status";

bool relay_status = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setRelay(bool new_status){
  relay_status = new_status;
    
  if (relay_status){
    digitalWrite(12, 0);
    delay(50);
    digitalWrite(12, 1);
  } else {
    digitalWrite(5, 0);
    delay(50);
    digitalWrite(5, 1);
  }

  client.publish(mqtt_status_topic, relay_status?"1":"0", true);

  //Serial.print("relay ");
  //Serial.println(relay_status?"on":"off");
}

void setup(void){
  Serial.begin(115200); 
  delay(200);

  // Setting the I/O pin modes

  pinMode(14, INPUT); // Push button
  pinMode(4, OUTPUT); // Blue LED
  pinMode(5, OUTPUT); // Relay off
  pinMode(12, OUTPUT); // Relay on

  delay(200);
  setRelay(true);
  setRelay(false);
  
  // Connecting to a WiFi network

  Serial.println();
  Serial.println("Connecting to WiFi");

  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected with ip ");
  Serial.println(WiFi.localIP());
  digitalWrite(4, 1);
 
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (length == 0) return;

  setRelay((char)payload[0] == '1');
}

void mqttReconnect() {
  String clientID = "OBISocket_";
  clientID += WiFi.macAddress();

  Serial.print("Connect to MQTT-Broker");
  if (client.connect(clientID.c_str())) {
    Serial.print("connected as clientID:");
    Serial.println(clientID);

    client.subscribe(mqtt_command_topic);
    client.publish(mqtt_status_topic, relay_status?"1":"0", true);
      
  } else {
    Serial.print("failed: ");
    Serial.println(client.state());
  }
}

bool inp_status = 1;

void loop(void){
  bool inp = digitalRead(14);
  if (inp == 0 and inp != inp_status) {
    setRelay(!relay_status);
  }
  inp_status = inp;
  
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  
  delay (50);
} 
