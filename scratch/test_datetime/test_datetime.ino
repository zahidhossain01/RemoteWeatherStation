#include "Arduino.h"

#include "WiFi.h"

#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6*3600;
const int daylightOffset_sec = 0;
struct tm timeinfo;



#include <Wire.h>
#include <HT_SSD1306Wire.h>



// Zahid's Apartment
#define WIFI_SSID "MensaUT"
#define WIFI_PASSWORD "beb9141125"

// utexas-iot
// #define WIFI_SSID "utexas-iot"
// #define WIFI_PASSWORD "89493324389382547686"



SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
void VextON(void){
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}
void VextOFF(void){
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void setup() {
    Serial.begin(115200);
    
    VextON();
    delay(100);
    factory_display.init();
    factory_display.setFont(ArialMT_Plain_10);

    factory_display.clear();
    factory_display.drawString(0, 0, "HELLO");
    factory_display.drawString(0, 10, "WORLD :)");
    factory_display.display();


    
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


    // timeClient.begin();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


void loop(){
  delay(1000);
  
  /* TIME STRUCT DEFINITION:
    struct tm {
      int8_t     tm_sec; // seconds after the minute - [ 0 to 59 ]
      int8_t     tm_min; // minutes after the hour - [ 0 to 59 ]
      int8_t     tm_hour; // hours since midnight - [ 0 to 23 ]
      int8_t     tm_mday; // day of the month - [ 1 to 31 ]
      int8_t     tm_wday; // days since Sunday - [ 0 to 6 ]
      int8_t     tm_mon; // months since January - [ 0 to 11 ]
      int16_t    tm_year; // years since 1900
      int16_t    tm_yday; // days since January 1 - [ 0 to 365 ]
      int16_t    tm_isdst; // Daylight Saving Time flag
    };
  */
  // https://cplusplus.com/reference/ctime/strftime/

  getLocalTime(&timeinfo);
  static char time_buffer[50];
  strftime(time_buffer, 50, "%F %H:%M:%S GMT%z", &timeinfo);
  Serial.println(time_buffer);
  // Formats that work:
  // Wednesday, November 15 2023 11:10:27 GMT-0600
  // 2023-11-15 11:10:27 GMT-0600

  Serial.println();

  factory_display.clear();

  factory_display.setFont(ArialMT_Plain_16);
  strftime(time_buffer, 50, "%I:%M:%S %p", &timeinfo);
  factory_display.drawString(0, 0, time_buffer);
  
  factory_display.setFont(ArialMT_Plain_10);
  strftime(time_buffer, 50, "%A, %b %e", &timeinfo);
  factory_display.drawString(0, 0+19, time_buffer);

  strftime(time_buffer, 50, "%Y", &timeinfo);
  factory_display.drawString(0, 19+13, time_buffer);

  strftime(time_buffer, 50, "GMT%z", &timeinfo);
  factory_display.drawString(0, 19+13+13, time_buffer);

  factory_display.display();

}

