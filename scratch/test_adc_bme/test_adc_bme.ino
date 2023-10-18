#include "Arduino.h"


#include <Adafruit_BME280.h>
#include <Wire.h>


// #define BME_SCK 47
// #define BME_MISO 48
// #define BME_CS 26
// #define BME_MOSI 33

#define BME_SCL 47
#define BME_SDA 48

#define SEALEVELPRESSURE_HPA (1013.25)

#define ADC_PIN 1

// "Make sure to put SDO to high to get address 0x77. This will work with Adafruit BME280 library perfectly." (think this is just for I2C)
// primary SPI ports are supposedly HSPI (SPI2) and VSPI (SPI3)
// http://community.heltec.cn/t/wifi-lora-32-v3-i2c-sda-scl/12029/4


Adafruit_BME280 bme; // I2C
TwoWire BME_Wire(1);
// Adafruit_BME280 bme(BME_CS); // hardware SPI
// Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI


// ESP32 uses 3.3v 12-bit ADC
int count = 0;
int adc_sum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  // Mcu.begin(); // what does this do exactly... pretty sure it's needed for LoRa, but is that it?
  
  pinMode(ADC_PIN, INPUT);

  // bool bmestatus = bme.begin();
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
}



void loop(){
  delay(500);

  // ADC
  int adc_val = analogRead(ADC_PIN);
  adc_sum += adc_val;
  count += 1;
  
  int adc_avg = 0;
  if(count == 20){
    adc_avg = adc_sum / count;
    // Serial.printf("ADC: %d\n", adc_avg);
    Serial.print("ADC avg: ");
    Serial.println(adc_avg);
    adc_sum = 0;
    count = 0;
  }


  // BME
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();

}

