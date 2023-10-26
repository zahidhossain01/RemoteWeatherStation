
#include "LoRaWan_APP.h"
#include "Arduino.h"

#include "WiFi.h"
#include <HTTPClient.h>

#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6*3600;
const int daylightOffset_sec = 3600;
struct tm timeinfo;

#include <NTPClient.h>

#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>

#include <Wire.h>
#include "HT_SSD1306Wire.h"


#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

// Zahid's Apartment
// #define WIFI_SSID "ARRIS-24A9"
// #define WIFI_PASSWORD "431010172038"

// utexas-iot
#define WIFI_SSID "utexas-iot"
#define WIFI_PASSWORD "89493324389382547686"

// HTTPClient http;
// String server_base_url = "https://test-esp-api-default-rtdb.firebaseio.com/data.json";

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t txNumber;

int16_t rssi,rxSize;

bool lora_idle = true;

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);

void setup() {
    Serial.begin(115200);
    Mcu.begin();
    
    txNumber=0;
    rssi=0;
  
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );



    // OLED stuff not working rn
    factory_display.connect();
    factory_display.init();
    factory_display.displayOn();
    
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress()); // F4:12:FA:43:85:4C

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi with IP: ");
    Serial.println(WiFi.localIP());

    // http.begin(server_base_url);

    // Test Cloud Firestore instead
    config.signer.test_mode = true;
    // config.api_key = key;
    // auth.user.email = email;
    // auth.user.password = pass;
    fbdo.setResponseSize(2048);
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // timeClient.begin();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}



void loop()
{
  if(lora_idle)
  {
    lora_idle = false;
    Serial.println("into RX mode");
    factory_display.drawString(0, 0, "into RX mode"); // not working rn
    Radio.Rx(0);
  }
  Radio.IrqProcess( );

}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    Radio.Sleep( );
    Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n",rxpacket,rssi,rxSize);
    lora_idle = true;

    // String received_string = String(rxpacket);
    // Serial.println(received_string);

    char* tem_s = strtok(rxpacket, ",");
    float temperature = strtof(tem_s, NULL);
    char* hum_s = strtok(NULL, ",");
    float humidity = strtof(hum_s, NULL);
    Serial.printf("Temp %0.2f F | Hum: %0.2f%\r\n", temperature, humidity);


    // int response_code = http.POST(String(rxpacket));
    // Serial.print("HTPP Response Code: ");
    // Serial.println(response_code);


    // Test Cloud Firestore instead
    if(!Firebase.ready()){return;}
    FirebaseJson content;

    // NEED TO SET TIMESTAMP SOMEHOW
    // timeClient.update();
    // String time = timeClient.getFormattedTime();
    String time = "00:00:00";
    // Serial.println(time);
    getLocalTime(&timeinfo);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    // TODO: iso-8601 time format?

    content.set("fields/time/stringValue", time);
    content.set("fields/temp/doubleValue", temperature);
    content.set("fields/humidity/doubleValue", humidity);
    // https://firestore.googleapis.com/v1/projects/remote-weather-station-31653/databases/(default)/documents/sensor_data
    // String doc_path = "projects/";
    // doc_path += "remote-weather-station-31653";
    // doc_path += "/databases/(default)/documents/sensor_data";

    // TODO: CHECK IF A READ IS OCCURING WHILE SENDING DATA?? THE PRINTF OF FBDO.PAYLOAD() ?


    String documentPath = "sensor_data";
    // don't send data for now
    // if (Firebase.Firestore.createDocument(&fbdo, "remote-weather-station-31653", "", documentPath.c_str(), content.raw())){
    //   Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    // }else{
    //   Serial.print("Firestore fail: ");
    //   Serial.println(fbdo.errorReason());
    // }
}