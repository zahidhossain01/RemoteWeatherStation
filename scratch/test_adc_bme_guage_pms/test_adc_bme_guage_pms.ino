

#include "Arduino.h"

#include <Wire.h>
#include <HT_SSD1306Wire.h>
#include <Adafruit_BME280.h>
#include <SparkFun_Weather_Meter_Kit_Arduino_Library.h>
#include <PMS.h>


#define BME_SCL 47
#define BME_SDA 48
#define SEALEVELPRESSURE_HPA (1013.25)

#define WIND_DIRECTION_PIN 1
#define ANEMOMETER_PIN 4
#define RAINFALL_PIN 2

#define PMS_RX 19
#define PMS_TX 20
// REMEMBER TO WIRE PMS-TX TO ESP-RX (and vice versa)

// Heltec LoRa V3 videos:
// https://www.youtube.com/watch?v=shJxD5fiEhs
// https://www.youtube.com/watch?v=sBYHlvLupxw
// https://www.youtube.com/watch?v=3x8vREn3mq0

// ESP32 uses 3.3v 12-bit ADC
// http://community.heltec.cn/t/wifi-lora-32-v3-i2c-sda-scl/12029/4
// ~\AppData\Local\Arduino15\packages\Heltec-esp32\hardware\esp32\0.0.7 | can find libraries here


SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}


TwoWire BME_Wire(1);
Adafruit_BME280 bme; // I2C

SFEWeatherMeterKit meter(WIND_DIRECTION_PIN, ANEMOMETER_PIN, RAINFALL_PIN);


PMS pms(Serial2);
PMS::DATA pms_data;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  // Mcu.begin(); // what does this do exactly... pretty sure it's needed for LoRa, but is that it?
  
  VextON();
	delay(100);
	factory_display.init();
	factory_display.clear();
	factory_display.display();

  factory_display.setFont(ArialMT_Plain_10);
  factory_display.drawString(0, 0, "You can do it \\^-^/ !");
	factory_display.display(); // need .display() after any drawString or etc operation I think

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

  Serial2.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX); // PMS Sensor
  // TODO: try passive mode
  // pms.activeMode();
  pms.passiveMode();
  pms.wakeUp();
  Serial.println("Wait 30s for PMS warmup");
  delay(30000);
  while (Serial2.available()) { Serial2.read(); } // Clear PMS Serial2 buffer
}



void loop(){
  delay(2000);

  pms.requestRead();
  if (pms.readUntil(pms_data)){
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(pms_data.PM_AE_UG_1_0);

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(pms_data.PM_AE_UG_2_5);

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(pms_data.PM_AE_UG_10_0);
  } else {
    Serial.println("NO PMS DATA");
  }

  // if(Serial2.available()){
  //   Serial.println("Serial2 available");
  //   Serial2.println("Hello World");
  // } else {
  //   Serial.println("Serial2 not available");
  // }

  Serial.println();

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

