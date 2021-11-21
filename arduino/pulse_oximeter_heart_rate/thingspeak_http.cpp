#include <WiFi.h>
#include "display.h"
#include "oximeter.h"

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* apiKey = "APIKEY";
const char* resource = "/update?api_key=";
const char* server = "api.thingspeak.com";

void Wifi_connect() {
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
  Serial.println(server);
  Serial.println("Uploading data to thingspeak");
  Serial.print("oxygen"); Serial.print(oxygen);
  Serial.print("beat"); Serial.print(beat);
  Serial.println("");

  WiFiClient client;

  if (client.connect(server, 80)) {
    Serial.println(F("connected"));
  }
  else  {
    Serial.println(F("connection failed"));
    return;
  }

  // Upload data to ThingSpeak via HTTP RESTful API
  Serial.print("Request resource: ");
  Serial.println(resource);
  client.print(String("GET ") + resource + apiKey + "&field1=" + oxygen + "&field2=" + beat +
               " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
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
