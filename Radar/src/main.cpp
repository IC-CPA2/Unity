#include <Arduino.h>
#include <Wire.h>
 
//analog read
const int Analog_channel_pin = 27;
int ADC_VALUE = 0;
float voltage_value = 0;

const byte LED_GPIO = 32; //read from A4

void setup() 
{
Serial.begin(9500);
pinMode(A0,INPUT);

// initialize digital pin LED_GPIO as an output.
pinMode(LED_GPIO, OUTPUT);
}

void loop() 
{

int found = 0;
for (int i =0; i<50; i++){  
ADC_VALUE = analogRead(Analog_channel_pin);
// Serial.print("ADC VALUE = ");
// Serial.println(ADC_VALUE);
voltage_value = (ADC_VALUE * 3.3 ) / (4095);
// Serial.print("Voltage = ");
// Serial.print(voltage_value);
// Serial.println(" volts");
if (voltage_value > 2){
  digitalWrite(LED_GPIO, HIGH);
  Serial.println("fan detected!");
  found = 1;
  break;
}

}
if(found==0){
  digitalWrite(LED_GPIO, LOW);
  Serial.println("no fan");
}


}

