#include "Arduino.h"
#include "LoRaWan_APP.h"

// #include <DHT_U.h>
// #include <DHT.h>

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

#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             5        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

double txNumber;

bool lora_idle=true;

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );

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
    Mcu.begin();

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

      factory_display.drawString(0, 0, "ERROR: BME280 SENSOR MISSING?");
      factory_display.display();

      while (1) delay(10);
    }

    meter.begin();
    // TODO: wind direction calibration, set cal values

    Serial2.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX); // PMS Sensor
    // pms.activeMode();
    pms.passiveMode();
    pms.wakeUp();
    Serial.println("Wait 30s for PMS warmup");
    delay(30000);
    while (Serial2.available()) { Serial2.read(); } // Clear PMS Serial2 buffer



    //**************** LoRa ****************
    txNumber=0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 
   }



void loop()
{
	if(lora_idle == true){
    delay(1000*2);

  
    pms.requestRead();
    bool pms_read_success = pms.readUntil(pms_data);
    if (!pms_read_success){
      Serial.println("NO PMS DATA");
    }

    float temp_c = bme.readTemperature();
    float temp_f = 1.8*temp_c + 32;
    float pressure = bme.readPressure()/100.0F; // hPa
    float humidity = bme.readHumidity();
    float speed = meter.getWindSpeed();
    float direction = meter.getWindDirection();
    float rain = meter.getTotalRainfall();
    float pms_1_0 = (float) pms_data.PM_AE_UG_1_0;
    float pms_2_5 = (float) pms_data.PM_AE_UG_2_5;
    float pms_10_0 = (float) pms_data.PM_AE_UG_10_0;


    // if(isnan(temp_f) || isnan (humidity)){
    //   Serial.println("Failed to read from DHT sensor!\r\n");
    //   return;
    // }
    Serial.println();
    Serial.println(temp_f);
    Serial.println(humidity);
    Serial.println(pressure);
    Serial.println(speed);
    Serial.println(direction);
    Serial.println(rain);
    Serial.println(pms_1_0); // (ug/m3)
    Serial.println(pms_2_5);
    Serial.println(pms_10_0);


   
		// Serial.printf("sending packet, length %d\r\n",txpacket, strlen(txpacket));

    if(pms_read_success){

      const int num_measures = 9;
      float* measurements[num_measures] = {&temp_f, &humidity, &pressure, 
                                            &speed, &direction, &rain,
                                            &pms_1_0, &pms_2_5,
                                            &pms_10_0};

      uint8_t payload_bytes[sizeof(float) * num_measures];

      for(int i = 0; i < num_measures; i++){
        memcpy(&payload_bytes[i*sizeof(float)], measurements[i], sizeof(float));
      }



		  Radio.Send(payload_bytes, num_measures*sizeof(float));	
      lora_idle = false;
      // old code (DELETE):
		  // sprintf(txpacket,"%0.2f,%0.2f",temp_f,humidity);
		  // Radio.Send( (uint8_t *)txpacket, strlen(txpacket) );
    }
	}

  Radio.IrqProcess( );
}

void OnTxDone( void )
{
	Serial.println("TX done......");
	lora_idle = true;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    Serial.println("TX Timeout......");
    lora_idle = true;
}