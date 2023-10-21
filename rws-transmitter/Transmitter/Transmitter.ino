#include "Arduino.h"
#include "LoRaWan_APP.h"

// #include <DHT_U.h>
// #include <DHT.h>

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SparkFun_Weather_Meter_Kit_Arduino_Library.h>

#define BME_SCL 47
#define BME_SDA 48
#define SEALEVELPRESSURE_HPA (1013.25)

#define WIND_DIRECTION_PIN 1
#define ANEMOMETER_PIN 4
#define RAINFALL_PIN 2

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

TwoWire BME_Wire(1);
Adafruit_BME280 bme; // I2C

SFEWeatherMeterKit meter(WIND_DIRECTION_PIN, ANEMOMETER_PIN, RAINFALL_PIN);

void setup() {
    Serial.begin(115200);
    Mcu.begin();

    // dht.begin();

    meter.begin();

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
	if(lora_idle == true)
	{
    // delay(1000*30);
    delay(1000*2);
    float temp_c = bme.readTemperature();
    float temp_f = 1.8*temp_c + 32;
    float pressure = bme.readPressure();
    float humidity = bme.readHumidity();
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

    float speed = meter.getWindSpeed();
    float direction = meter.getWindDirection();
    float rain = meter.getTotalRainfall();
    
    // int adc_val = analogRead(ADC_PIN);
    if(isnan(temp_f) || isnan (humidity)){
      Serial.println("Failed to read from DHT sensor!\r\n");
      return;
    }
    Serial.println();
    Serial.println(temp_f);
    Serial.println(humidity);
    Serial.println(speed);

    // TODO: change txpacket and rxpacket to uint8_t instead of chars
    // TODO: figure out how to split float into uint8_t's and later reassemble

		sprintf(txpacket,"%0.2f,%0.2f",temp_f,humidity);  //start a package
   
		Serial.printf("sending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));

		Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //send the package out	
    lora_idle = false;
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