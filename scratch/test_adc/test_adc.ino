#include "Arduino.h"

#define ADC_PIN 1

// ESP32 uses 3.3v 8-bit(?)/12-bit(?) ADC
int count = 0;
int adc_sum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup Begin");
  // Mcu.begin(); // what does this do exactly... pretty sure it's needed for LoRa, but is that it?
  
  pinMode(ADC_PIN, INPUT);
}



void loop(){
  delay(50);

  int adc_val = analogRead(ADC_PIN);
  adc_sum += adc_val;
  count += 1;
  
  int adc_avg = 0;
  if(count == 20){
    adc_avg = adc_sum / count;
    // Serial.printf("ADC: %d\n", adc_avg);
    Serial.print("ADC avg:");
    Serial.println(adc_avg);
    adc_sum = 0;
    count = 0;
  }

}

