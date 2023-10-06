#include "Arduino.h"

#define ADC_PIN 1

// ESP32 uses 3.3v 8-bit(?)/12-bit(?) ADC
int count = 0;
int adc_sum = 0;

void setup() {
  Serial.begin(115200);
  // Mcu.begin();
  
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
    Serial.print("ADC:");
    Serial.println(adc_avg);
    adc_sum = 0;
    count = 0;
  }

}

