/*
 * Project lab3
 * Description: 
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: July 22, 2019
 */
#include "Particle.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"
#include "JsonParserGeneratorRK.h"

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

int toggleLed(String args);
void createEventPayload(int temp, int humidity, double light);

int temp, humidity;
double currentLightLevel;

unsigned long previousCheckMillis = 0;
unsigned long checkInterval = 30000;

void setup()
{
  Serial.begin(9600);

  dht.begin();

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  pinMode(A0, INPUT);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);

  Particle.function("toggleLed", toggleLed);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousCheckMillis > checkInterval)
  {
    previousCheckMillis = millis();

    temp = (int)dht.getTempFarenheit();
    humidity = (int)dht.getHumidity();

    Serial.printlnf("Temp: %f", temp);
    Serial.printlnf("Humidity: %f", humidity);

    double lightAnalogVal = analogRead(A0);
    currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

    createEventPayload(temp, humidity, currentLightLevel);

    if (currentLightLevel > 50)
    {
      Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
    }
  }
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}

void createEventPayload(int temp, int humidity, double light)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("temp", temp);
    jw.insertKeyValue("humidity", humidity);
    jw.insertKeyValue("light", light);
  }

  Particle.publish("env-vals", jw.getBuffer(), PRIVATE);
}