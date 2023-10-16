#include "Arduino.h"


#include <Adafruit_BME280.h>
// #include <SPI.h>

#define BME_SCK 36
#define BME_MISO 33
#define BME_MOSI 35
#define BME_CS 34

#define SEALEVELPRESSURE_HPA (1013.25)

#define ADC_PIN 1

//Adafruit_BME280 bme; // I2C
// Adafruit_BME280 bme(BME_CS); // hardware SPI
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI


// ESP32 uses 3.3v 8-bit(?)/12-bit(?) ADC
int count = 0;
int adc_sum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  // Mcu.begin(); // what does this do exactly... pretty sure it's needed for LoRa, but is that it?
  
  pinMode(ADC_PIN, INPUT);

  bool bmestatus = bme.begin();  
  if (!bmestatus) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}



void loop(){
  delay(50);

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

