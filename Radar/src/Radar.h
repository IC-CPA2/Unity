#include <Arduino.h>
#include <Wire.h>

class Radar
{

//analog read
private:
const int Analog_channel_pin = 33;
const byte LED_GPIO = 32;

public:
int ADC_VALUE = 0;
float voltage_value = 0;

void setup() {
  Serial.begin(9500);
// initialize analog pin A3 as an input.
pinMode(A3,INPUT);
// initialize digital pin LED_BUILTIN as an output.
pinMode(LED_GPIO, OUTPUT);
}

  bool fan_detect(){
    for (int i = 0; i < 50; i++){  
      ADC_VALUE = analogRead(Analog_channel_pin);
      voltage_value = (ADC_VALUE * 3.3 ) / (4095);

        if (voltage_value > 1.8 ){
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
