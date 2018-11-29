#include <DHT.h>
#include <Seeed_BMP280.h>
#include <Wire.h>
#include "Adafruit_SI1145.h"

#define DHTTYPE DHT11
#define DHT_PIN A0
#define luminancePin A1
DHT dht(DHT_PIN, DHTTYPE);

BMP280 bmp280;
Adafruit_SI1145 uv = Adafruit_SI1145();

//Inialisation
float humi = 0.0;
float temp = 0.0;
float lumi = 0.0;
float pressure = 0.0;
float alt = 0.0;
float UVindex = 0.0;

void setup() {
  Serial.begin(115200);
  if (!bmp280.init()) {
    Serial.println("Error with BMP280 sensor");
  }
  if (! uv.begin()) {
    Serial.println("Error with uv sensor");
    while (1);
  }
}
void loop()
{
  // recover data
  pressure = bmp280.getPressure();
  alt = bmp280.calcAltitude(pressure);
  temp = bmp280.getTemperature();
  humi = dht.readHumidity();
  lumi = analogRead(luminancePin);
  UVindex = uv.readUV();
  UVindex /= 100.0;

  Serial.print("a");
  Serial.print(humi);
  Serial.print("b");
  Serial.print("e");
  Serial.print(lumi);
  Serial.print("f");
  Serial.print("g");
  Serial.print(pressure);
  Serial.print("h");
  Serial.print("i");
  Serial.print(alt);
  Serial.print("j");
  Serial.print("k");
  Serial.print(temp);
  Serial.print("l");
  Serial.print("q");
  Serial.print(UVindex);
  Serial.println("r");
  delay(5000);
}
