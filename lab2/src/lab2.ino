/*
 * Project lab2
 * Description:
 * Author:
 * Date:
 */
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "Grove_ChainableLED.h"

DHT dht(D2);
ChainableLED leds(A4, A5, 1);

int temp, humidity;
double currentLightLevel;

void setup()
{
  Serial.begin(9600);

  pinMode(A0, INPUT);

  dht.begin();

  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.function("toggleLed", toggleLed);
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}

void loop()
{
  temp = (int)dht.getTempFarenheit();
  humidity = (int)dht.getHumidity();

  Serial.printlnf("Temp: %i", temp);
  Serial.printlnf("Humidity: %i", humidity);

  double lightAnalogVal = analogRead(A0);
  currentLightLevel = map(lightAnalogVal, 0.0, 4095.0, 0.0, 100.0);

  if (currentLightLevel > 50)
  {
    Particle.publish("light-meter/level", String(currentLightLevel), PRIVATE);
  }

  delay(2000);
}