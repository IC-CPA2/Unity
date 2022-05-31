#include <SPI.h>

#define VSPI_MISO   MISO
#define VSPI_MOSI   MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS

SPISettings settings(100000, MSBFIRST, SPI_MODE0);

uint8_t spi_counter[6]; // [0] = c20, [1] = c21, [2] = c22, [3] = c23, [4] = c24, [5] = c25
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;

void calcDistance();
void resetCounter();

//vspi default pins SCLK = 18, MISO = 19, MOSI = 23, SS = 5 
void setup() {

  Serial.begin(9600);

  // Setup SPI stuff
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  resetCounter();
  spi_returnval = 0;
}

void loop() {

  // Transfer stuff
  SPI.beginTransaction(settings);
  digitalWrite(VSPI_SS, LOW);
  spi_val = SPI.transfer16(spi_returnval);
  spi_returnval = 0;
  digitalWrite(VSPI_SS, HIGH);
  SPI.endTransaction();

  // Processing data received
  if (spi_val == 2048){
    Serial.print("Drone rotate right \n");
    resetCounter();
  }
  if (spi_val == 4096){
    Serial.print("Drone rotate left\n");
    resetCounter();
  }
  if (spi_val == 8192){
    Serial.print("Drone move backwards \n");
    resetCounter();
  }
  if (spi_val == 16384){
    Serial.print("Drone move forwards \n");
    resetCounter();
  }
  if (spi_val > 32768){
    Serial.print("Calculating... \n");
    spi_val -= 32768;
    spi_val >>= 7;
    spi_reg = spi_val & 7;
    spi_val >>= 3;
    switch(spi_reg)
    {
      case 0:
        Serial.print("We have a pink ball \n");
        calcDistance();
        break;
      case 1:
        Serial.print("We have a yellow ball \n");
        calcDistance();
        break;
      case 2:
        Serial.print("We have a green ball \n");
        Serial.print("Distance is: ");
        Serial.println(spi_val);
        break;
      case 3:
        Serial.print("We have a blue ball \n");
        Serial.print("Distance is: ");
        Serial.println(spi_val);
        break;
      default:
        Serial.print("Invalid ball detected \n");
    }
  }
  delay(10);
}

void resetCounter(){
  for(int i = 0; i < 6; i++){
    spi_counter[i] = 0;
  }
}

void calcDistance() 
{
  for(int i = 0; i < 6; i++){
    if(spi_val = i+20){
      spi_counter[i]++;
      if(spi_counter[i] == 100){
        Serial.print("Distance is: ");
        Serial.println(i+20);
        spi_returnval = 32868;
        resetCounter();
        delay(10000);
      }
    }
  }
}
