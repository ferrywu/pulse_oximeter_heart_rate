/*
   Install libraries

   Adafruit SSD1306
   MAX30105
   ESP32Servo

   References

   20210705，https://youtu.be/ghTtpUTSc4o
   https://datasheets.maximintegrated.com/en/ds/MAX30102.pdf
   https://pdfserv.maximintegrated.com/en/an/AN6409.pdf
*/

#include "display.h"
#include "oximeter.h"
#include "thingspeak_http.h"

int lastUpload = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("System Start");
  display_init();
  delay(3000);
  oximeter_init();
  thingspeak_http_init();
}

void loop() {
  int beatAvg;
  double ESpO2;

  if (oximeter_get_info(&beatAvg, &ESpO2)) {
    // upload to ThingSpeak every 20 seconds
    if (millis() - lastUpload > 20000) {
      thingspeak_http_upload(ESpO2, beatAvg);
      lastUpload = millis();
    }
  } else {
    // reset ThingSpeak upload timer
    lastUpload = millis();
  }
}
