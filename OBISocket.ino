#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

extern "C" {
#include "user_interface.h"
}

// Do we have power measurement with an ACS712 (5A)
//#define ENABLE_POWER_MEASUREMENT 1

// WiFi Settings

const char* ssid = "YourSSID";
const char* password = "YourPassword";

WiFiClient espClient;

// MQTT Settings

const char* mqtt_server = "YourBroker";
PubSubClient client(espClient);

// Switching
bool relay_status = false;
bool inp_status = 1;

const char* mqtt_command_topic = "obisocket/0/command";
const char* mqtt_status_topic = "obisocket/0/status";

// Power measurement
#ifdef ENABLE_POWER_MEASUREMENT
#define SUM_MAX 600
#define ADC_MAX 1024

int count = 0;
int adc_avr = 0;
uint16_t vals[ADC_MAX+1];
unsigned long time_m;
unsigned long time_mn;
float Ieff;
float P;
float W;

const char* mqtt_power_topic = "obisocket/0/power";
const char* mqtt_energy_topic = "obisocket/0/energy";
#endif

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

#ifdef ENABLE_POWER_MEASUREMENT
void do_measurements(int n) {
  uint16_t last_adc_read;
  
  for (;n > 0; n--) {
    count++;
    if (count >= SUM_MAX) {
      float U, Ueff;
      float Sum_U_Square= 0.0;
  
      count = 0;
      adc_avr = adc_avr/SUM_MAX;
  
      for (int i = 0; i < ADC_MAX+1; i++) {
        if (vals[i] != 0) {
          Serial.print(i);Serial.print(": ");
          Serial.print(vals[i]);Serial.println(" ");

          if (abs(i-adc_avr) > 2) {
            U = ((float)(i - adc_avr))/1024 *5000;
            Sum_U_Square += U*U*vals[i]; 
          }
          vals[i] = 0;
        }
      }
      Ueff = sqrt(Sum_U_Square/SUM_MAX);
      Ieff = Ueff/185.0;
      P = Ieff*230;
      time_mn = millis();
      W += P * (time_mn - time_m)/3600000.0;
      time_m = time_mn;

/*      
       Serial.print(String(adc_avr)+"  ");
      Serial.print(String(Ueff)+"mV  ");
      Serial.print(String(Ieff)+"A  ");
      Serial.print(String(P)+"W ");
      Serial.println(String(W)+"Wh");
*/  

      client.publish(mqtt_power_topic, String(P).c_str(), true);
      client.publish(mqtt_energy_topic, String(W).c_str(), true);
            
      adc_avr = 0;
    }

    last_adc_read = analogRead(A0);
    if (last_adc_read > ADC_MAX) {
      Serial.println("ADC error");
      continue;
    }
    vals[last_adc_read]++; 
    adc_avr += last_adc_read;
    delay(2);
  }
}
#endif

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
#ifdef ENABLE_POWER_MEASUREMENT
  wifi_set_sleep_type(NONE_SLEEP_T);
#endif
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

  time_m = millis();
}

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

#ifdef ENABLE_POWER_MEASUREMENT
  do_measurements(50);
#else
  delay(50);
#endif
}
