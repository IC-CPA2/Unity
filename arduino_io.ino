#include <SPI.h>

#define VSPI_MISO   MISO
#define VSPI_MOSI   MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS

SPISettings settings(100000, MSBFIRST, SPI_MODE0);

uint8_t spi_counter[6];
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;

void calcDistance();
void resetCounter();

void setup() {
  Serial.begin(9600);
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  spi_returnval = 0;
}

void loop(){
  int nocol = 1;
  int redcount = 1;
  int bluecount = 1;
  int pinkcount = 1;
  int orangecount = 1;

  int reddist = 0;
  int bluedist = 0;
  int pinkdist = 0;
  int orgdist = 0;

  int redpos = 0;
  int bluepos = 0;
  int pinkpos = 0;
  int orgpos = 0;
  
  int buildingcount = 0;
  int buildingdist = 0;

  for(int i=0; i<10000; i++){
    
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();

    int col = spi_val%8;
    spi_val = spi_val/8;
    int pos = spi_val%8;
    spi_val = spi_val/8;
    int dist = spi_val;

    if(col==0){
      nocol++;
    }
    else if(col==1){
      redcount++;
      reddist = reddist + dist;
      redpos = redpos + pos;
    }
    else if(col==2){
      bluecount++;
      bluedist = bluedist + dist;
      bluepos = bluepos + pos;
    }
    else if(col==3){
      orangecount++;
      orgdist = orgdist + dist;
      orgpos = orgpos + pos;
    }
    else if(col==4){
      pinkcount++;
      pinkdist = pinkdist + dist;
      pinkpos = pinkpos + pos;
    }
    else if(col==7){
      buildingcount++
      buildingdist = buildingdist + dist;
    }
  }
  Serial.println("Red: ");
  Serial.println(reddist/redcount);
  Serial.println(redpos/redcount);

  Serial.println("Orange: ");
  Serial.println(orgdist/orgcount);
  Serial.println(orgpos/orgcount);

  Serial.println("Pink: ");
  Serial.println(pinkdist/pinkcount);
  Serial.println(pinkpos/pinkcount);

  Serial.println("Blue: ");
  Serial.println(bluedist/bluecount);
  Serial.println(bluepos/bluecount);

  Serial.println("Building: ");
  Serial.println(buildingdist/buildingcount);
  
}
