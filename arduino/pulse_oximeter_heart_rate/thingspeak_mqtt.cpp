#include <WiFi.h>
#include <PubSubClient.h>
#include "display.h"
#include "oximeter.h"

#define ssid "SSID"
#define password "PASSWORD"

#define mqtt_server "mqtt3.thingspeak.com"
#define mqtt_port 1883
#define mqtt_clientid "CLIENTID"
#define mqtt_username "USERNAME"
#define mqtt_password "PASSWORD"
#define mqtt_channelid "CHANNELID"

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

static void Wifi_connect() {
  display_wifi_connecting(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display_wifi_progress();
  }

  display_wifi_connected(WiFi.localIP().toString().c_str());
  delay(500);
}

static void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void thingspeak_mqtt_init(void) {
  Wifi_connect();
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(callback);
}

void thingspeak_mqtt_upload(double oxygen, int beat) {
  while (!mqtt_client.connected()) {
    Serial.print("Connecting to ");
    Serial.println(mqtt_server);

    // Attempt to connect
    if (mqtt_client.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to the topic when the connection is successful
      String subscribe = String("channels/") + String(mqtt_channelid) + String("/subscribe");
      mqtt_client.subscribe(subscribe.c_str());
    } else {
      Serial.print("connection failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  mqtt_client.loop();

  Serial.println("Uploading data to thingspeak");
  Serial.print("oxygen "); Serial.print(oxygen);
  Serial.print(" beat "); Serial.print(beat);
  Serial.println("");

  // Upload data to ThingSpeak via MQTT
  String publish = String("channels/") + String(mqtt_channelid) + String("/publish");
  String data = "field1=" + String(oxygen) + "&field2=" + String(beat);
  mqtt_client.publish(publish.c_str(), data.c_str());
}
