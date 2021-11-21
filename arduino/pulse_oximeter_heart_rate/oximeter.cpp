#include <Wire.h>
#include "MAX30105.h"           // MAX3010x library
#include "heartRate.h"          // Heart rate calculating algorithm
#include "display.h"

MAX30105 particleSensor;
// heart beat variables
const byte RATE_SIZE = 10; // size of heart beat array
byte rates[RATE_SIZE]; // heart beat array
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

// oximeter variables
double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;

double SpO2 = 0;
double ESpO2 = 90.0; // initial value
double FSpO2 = 0.7;  // filter factor for estimated SpO2
double frate = 0.95; // low pass filter for IR/red LED value to eliminate AC component
int i = 0;
int Num = 30; // sample rate
#define FINGER_ON 7000 // min value of ir�]check if finger on�^
#define MINIMUM_SPO2 90.0 // min value of oxygen

void oximeter_init(void) {
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
  {
    Serial.println("Cannot find MAX30102");
    while (1);
  }

  byte ledBrightness = 0x7F; // Options: 0=Off to 255=50mA
  byte sampleAverage = 4; // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; // Options: 1 = Red only(heart rate), 2 = Red + IR(oximeter)
  // Options: 1 = IR only, 2 = Red + IR on MH-ET LIVE MAX30102 board
  int sampleRate = 800; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 215; // Options: 69, 118, 215, 411
  int adcRange = 16384; // Options: 2048, 4096, 8192, 16384
  // Set up the wanted parameters
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings
  particleSensor.enableDIETEMPRDY();

  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
}

bool oximeter_get_info(int *pBeatAvg, double *pESpO2) {
  bool isFingerOn = false;
  long irValue = particleSensor.getIR();    // Reading the IR value it will permit us to know if there's a finger on the sensor or not

  // check if finger on
  if (irValue > FINGER_ON ) {
    display_oximeter_info(false, beatAvg, ESpO2);
    // check if heart beat
    if (checkForBeat(irValue) == true) {
      display_oximeter_info(true, beatAvg, ESpO2);
      long delta = millis() - lastBeat; // heart beat time difference
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0); // avarage of heart beat rate
      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        // heart beat rate should between 20 and 255
        rates[rateSpot++] = (byte)beatsPerMinute; // store heart beat array
        rateSpot %= RATE_SIZE;
        // calcuate avarage heart beat rate
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++) beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

    // calcuate oximeter
    uint32_t ir, red ;
    double fred, fir;
    particleSensor.check(); //Check the sensor, read up to 3 samples
    if (particleSensor.available()) {
      i++;
      red = particleSensor.getFIFOIR(); // read red light
      ir = particleSensor.getFIFORed(); // read ir
      //Serial.println("red=" + String(red) + ",IR=" + String(ir) + ",i=" + String(i));
      fred = (double)red;
      fir = (double)ir;
      avered = avered * frate + (double)red * (1.0 - frate); // average red level by low pass filter
      aveir = aveir * frate + (double)ir * (1.0 - frate); // average IR level by low pass filter
      sumredrms += (fred - avered) * (fred - avered); // square sum of alternate component of red level
      sumirrms += (fir - aveir) * (fir - aveir); // square sum of alternate component of IR level
      if ((i % Num) == 0) {
        double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
        SpO2 = -23.3 * (R - 0.4) + 100;
        ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2; // low pass filter
        if (ESpO2 <= MINIMUM_SPO2) ESpO2 = MINIMUM_SPO2; // indicator for finger detached
        if (ESpO2 > 100) ESpO2 = 99.9;
        Serial.print("Oxygen % = "); Serial.println(ESpO2);
        sumredrms = 0.0; sumirrms = 0.0; SpO2 = 0;
        i = 0;
      }
      particleSensor.nextSample(); // We're finished with this sample so move to next sample
    }

    isFingerOn = true;
  } else {
    // clear heart beat data
    for (byte rx = 0 ; rx < RATE_SIZE ; rx++) rates[rx] = 0;
    beatAvg = 0; rateSpot = 0; lastBeat = 0;
    // clear oximeter data
    avered = 0; aveir = 0; sumirrms = 0; sumredrms = 0;
    SpO2 = 0; ESpO2 = 90.0;
    isFingerOn = false;
    display_wait_for_finger();
  }

  if (pBeatAvg) *pBeatAvg = beatAvg;
  if (pESpO2) *pESpO2 = ESpO2;
  return isFingerOn;
}
