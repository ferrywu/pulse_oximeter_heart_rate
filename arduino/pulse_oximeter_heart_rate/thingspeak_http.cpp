#include <WiFi.h>
#include "display.h"
#include "oximeter.h"

#define ssid "SSID"
#define password "PASSWORD"

#define http_server "api.thingspeak.com"
#define http_port 80
#define http_update_key "UPDATE_KEY"
#define http_update_request "/update?api_key="

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

void thingspeak_http_init(void) {
  Wifi_connect();
}

void thingspeak_http_upload(double oxygen, int beat) {
  Serial.print("Connecting to ");
  Serial.println(http_server);
  Serial.println("Uploading data to thingspeak");
  Serial.print("oxygen "); Serial.print(oxygen);
  Serial.print(" beat "); Serial.print(beat);
  Serial.println("");

  WiFiClient client;

  if (client.connect(http_server, http_port)) {
    Serial.println(F("connected"));
  }
  else  {
    Serial.println(F("connection failed"));
    return;
  }

  // Upload data to ThingSpeak via HTTP RESTful API
  client.print(String("GET ") + http_update_request + http_update_key + "&field1=" + oxygen + "&field2=" + beat +
               " HTTP/1.1\r\n" +
               "Host: " + http_server + "\r\n" +
               "Connection: close\r\n\r\n");

  int timeout = 5 * 10; // 5 seconds
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }

  if(!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  Serial.println("");

  Serial.println("closing connection");
  client.stop();
}
