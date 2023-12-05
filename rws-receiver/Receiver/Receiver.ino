#include "Arduino.h"

#include "LoRaWan_APP.h"
#include "WiFi.h"
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <HT_SSD1306Wire.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6*3600;
const int daylightOffset_sec = 0;
struct tm timeinfo;




#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

// #define WIFI_SSID "MensaUT"
// #define WIFI_PASSWORD "beb9141125"

// #define WIFI_SSID "utexas-iot"
// #define WIFI_PASSWORD "89493324389382547686"

// #define WIFI_SSID "ZPixel"
// #define WIFI_PASSWORD "zinternet01"

#define WIFI_SSID "asushotspot"
#define WIFI_PASSWORD "macsucks"


char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
static RadioEvents_t RadioEvents;
int16_t txNumber;
int16_t rssi,rxSize;
bool lora_idle = true;

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
void VextON(void){
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}
void VextOFF(void){
  //Vext default OFF{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


void setup() {
    Serial.begin(115200);
    Mcu.begin();
    
    VextON();
    delay(100);
    factory_display.init();
    factory_display.setFont(ArialMT_Plain_10);

    factory_display.clear();
    factory_display.drawString(0, 0, "HELLO");
    factory_display.drawString(0, 10, "CRUEL WORLD :)");
    factory_display.display();


    txNumber=0;
    rssi=0;
  
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );


    
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


    // Cloud Firestore
    config.signer.test_mode = true;
    // config.api_key = key;
    // auth.user.email = email;
    // auth.user.password = pass;
    fbdo.setResponseSize(2048);
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}



void loop(){
  if(lora_idle){
    lora_idle = false;
    Serial.println("into RX mode");
    // factory_display.drawString(0, 0, "into RX mode"); // not working rn
    // factory_display.display();

    Radio.Rx(0);
  }
  Radio.IrqProcess( );

}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
    rssi=rssi;
    rxSize=size;

    // memcpy(rxpacket, payload, size ); // not actually using rxpacket for now, just payload
    // rxpacket[size]='\0';
    // String received_string = String(rxpacket);
    // Serial.println(received_string);

    Radio.Sleep( );
    Serial.printf("\r\nreceived packet with rssi %d, length %d\r\n",rssi,rxSize);
    lora_idle = true;

    

    float temp_f;
    float humidity;
    float pressure;
    float speed;
    float direction;
    float rain;
    float pms_1_0;
    float pms_2_5;
    float pms_10_0;
    const int num_measures = 9;
    float* measurements[num_measures] = {&temp_f, &humidity, &pressure, 
                                          &speed, &direction, &rain,
                                          &pms_1_0, &pms_2_5,
                                          &pms_10_0};

    for(int i = 0; i < num_measures; i++){
      memcpy(measurements[i], &payload[i*sizeof(float)], sizeof(float));
    }

    // TIMESTAMP
    // https://forum.arduino.cc/t/strftime-to-useable-variables/963093
    // TODO: iso-8601 time format?
    getLocalTime(&timeinfo);
    static char timestamp[50];
    strftime(timestamp, 50, "%F %H:%M:%S GMT%z", &timeinfo);
    Serial.println(timestamp);
    // Serial.println(temp_f);
    // Serial.println(humidity);
    // Serial.println(pressure);
    // Serial.println(speed);
    // Serial.println(direction);
    // Serial.println(rain);
    // Serial.println(pms_1_0); // (ug/m3)
    // Serial.println(pms_2_5);
    // Serial.println(pms_10_0);
    Serial.println();

    char temp_string[10];
    char humidity_string[10];
    snprintf(temp_string, sizeof(temp_string),         "Temperature:  %.2f [ºF]", temp_f);
    snprintf(humidity_string, sizeof(humidity_string), "Humidity:     %.2f [%]", humidity);
    char time_display_buf[50];
    factory_display.clear();
    factory_display.setFont(ArialMT_Plain_16);
    strftime(time_display_buf, 50, "%I:%M:%S %p", &timeinfo);
    factory_display.drawString(0, 0, time_display_buf);
    factory_display.setFont(ArialMT_Plain_10);
    strftime(time_display_buf, 50, "%A, %b %e %Y", &timeinfo);
    factory_display.drawString(0, 19, time_display_buf);
    factory_display.drawString(0, 19+13, temp_string);
    factory_display.drawString(0, 19+13+10, humidity_string);
    factory_display.display();


    
    

    if(!Firebase.ready()){return;}
    FirebaseJson content;
    content.set("fields/time/stringValue", timestamp);
    content.set("fields/temp/doubleValue",temp_f);
    content.set("fields/humidity/doubleValue",humidity);
    content.set("fields/pressure/doubleValue", pressure);
    content.set("fields/speed/doubleValue", speed);
    content.set("fields/direction/doubleValue", direction);
    content.set("fields/rain/doubleValue", rain);
    content.set("fields/pms_1_0/doubleValue", pms_1_0);
    content.set("fields/pms_2_5/doubleValue", pms_2_5);
    content.set("fields/pms_10_0/doubleValue", pms_10_0);

    String documentPath = "sensor_data";
    if (Firebase.Firestore.createDocument(&fbdo, "remote-weather-station-31653", "", documentPath.c_str(), content.raw())){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    }else{
      Serial.print("Firestore fail: ");
      Serial.println(fbdo.errorReason());
    }
}

