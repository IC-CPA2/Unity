#include <Arduino.h>
#include <Wire.h>

class Radar
{

//analog read
private:
const int Analog_channel_pin = 27;
const byte LED_GPIO = 32;

public:
int ADC_VALUE = 0;
float voltage_value = 0;

void setup() {
  Serial.begin(9500);
// initialize analog pin A0 as an input.
pinMode(A0,INPUT);
// initialize digital pin LED_BUILTIN as an output.
pinMode(LED_GPIO, OUTPUT);
}

  bool fan_detect(){
    for (int i = 0; i < 50; i++){  
      ADC_VALUE = analogRead(Analog_channel_pin);
      voltage_value = (ADC_VALUE * 3.3 ) / (4095);

        if (voltage_value > 2 ){
          digitalWrite(LED_GPIO, HIGH);
          Serial.println("fan detected!");
          return true;
          break;
        }

        else{
          digitalWrite(LED_GPIO, LOW);
          Serial.println("no fan");
          return false;
        }
    }
    return false;
  }

};

// void loop() 
// {
//  int found = 0;

// for (int i = 0; i < 50; i++){  
// ADC_VALUE = analogRead(Analog_channel_pin);
// Serial.print("ADC VALUE = ");
// Serial.println(ADC_VALUE);
// voltage_value = (ADC_VALUE * 3.3 ) / (4095);
// Serial.print("Voltage = ");
// Serial.print(voltage_value);
// Serial.println(" volts");

//for (int j = 0; j < 5;){
  // int count = 0;
  // while (count < 5){
    // if (voltage_value > 2 ){
      // count++;
    // }  
  // }

  //while (count = 5){
//   digitalWrite(LED_GPIO, HIGH);
//   Serial.println("fan detected!");
//   found = 1;
//   break;
//   }

//   if(found == 0){
//     digitalWrite(LED_GPIO, LOW);
//     Serial.println("no fan");
//   }
// }

// }


