#include "Arduino.h"

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SparkFun_Weather_Meter_Kit_Arduino_Library.h>


#define BME_SCL 47
#define BME_SDA 48
#define SEALEVELPRESSURE_HPA (1013.25)

#define WIND_DIRECTION_PIN 1
#define ANEMOMETER_PIN 4
#define RAINFALL_PIN 2

// ESP32 uses 3.3v 12-bit ADC
// http://community.heltec.cn/t/wifi-lora-32-v3-i2c-sda-scl/12029/4


TwoWire BME_Wire(1);
Adafruit_BME280 bme; // I2C

SFEWeatherMeterKit meter(WIND_DIRECTION_PIN, ANEMOMETER_PIN, RAINFALL_PIN);


void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  // Mcu.begin(); // what does this do exactly... pretty sure it's needed for LoRa, but is that it?
  

  BME_Wire.begin(BME_SDA, BME_SCL, 100000);
  bool bmestatus = bme.begin(0x76, &BME_Wire);  
  if (!bmestatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  meter.begin();
  // TODO: wind direction calibration, set cal values
}



void loop(){
  delay(2000);



  // BME:

  // Serial.print("Temperature = ");
  // Serial.print(bme.readTemperature());
  // Serial.println(" *C");
  
  float temp_c = bme.readTemperature();
  float pressure = bme.readPressure();
  float humidity = bme.readHumidity();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  float speed = meter.getWindSpeed();
  float direction = meter.getWindDirection();
  float rain = meter.getTotalRainfall(); // there's also getRainfallCounts... same with wind speed...

  Serial.print("Temperature = ");
  Serial.print(1.8 * temp_c + 32);
  Serial.println(" *F");
  
  Serial.print("Pressure = ");
  Serial.print(pressure / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Speed = ");
  Serial.println(speed);
  Serial.print("Direction = ");
  Serial.println(direction);
  Serial.print("Rainfall = ");
  Serial.println(rain);

  Serial.println();
}

