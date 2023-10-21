#include "Arduino.h"

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SparkFun_Weather_Meter_Kit_Arduino_Library.h>


#define BME_SCL 47
#define BME_SDA 48
#define SEALEVELPRESSURE_HPA (1013.25)


// ESP32 uses 3.3v 12-bit ADC
// http://community.heltec.cn/t/wifi-lora-32-v3-i2c-sda-scl/12029/4


TwoWire BME_Wire(1);
Adafruit_BME280 bme; // I2C



void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  

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

  // meter.begin();
  // TODO: wind direction calibration, set cal values
}



void loop(){
  delay(2000);


  
  float temp_c = bme.readTemperature();
  float temp_f = 1.8*temp_c + 32;
  float pressure = bme.readPressure();
  float humidity = bme.readHumidity();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  const int num_measures = 4;
  float* measurements[num_measures] = {&temp_f, &pressure, &humidity, &altitude};
  uint8_t payload_bytes[sizeof(float) * num_measures];

  for(int i = 0; i < num_measures; i++){
    memcpy(&payload_bytes[i*sizeof(float)], measurements[i], sizeof(float));
  }

  
  float f1, f2, f3, f4;
  float* copied_floats[] = {&f1, &f2, &f3, &f4};
  for(int i = 0; i < num_measures; i++){
    memcpy(copied_floats[i], &payload_bytes[i*sizeof(float)], sizeof(float));
  }

  Serial.println("Sensor Readings:");
  Serial.print("Temperature = ");
  Serial.print(temp_f);
  Serial.println(" [F]");
  
  Serial.print("Pressure = ");
  Serial.print(pressure);
  // Serial.print(pressure / 100.0F);
  Serial.println(" [hPa/100]");

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" [%]");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" [m]");

  Serial.println("\nPayload Bytes:");
  for(int i = 0; i < num_measures; i++){
    for(int b = 0; b < sizeof(float); b++){
      Serial.print(payload_bytes[i*sizeof(float) + b]);
      Serial.print("\t");
    }
    Serial.println();
  }

  Serial.println("\nReconstructed Floats:");
  for(int i = 0; i < num_measures; i++){
    Serial.println(*copied_floats[i]);
  }


  // Serial.print("Speed = ");
  // Serial.println(speed);
  // Serial.print("Direction = ");
  // Serial.println(direction);
  // Serial.print("Rainfall = ");
  // Serial.println(rain);

  Serial.println();
}

