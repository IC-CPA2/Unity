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
  int nocol = 0;
  int redcount = 0;
  int bluecount = 0;
  int pinkcount = 0;
  int orangecount = 0;
  int greencount = 0;

  int dist = 0;
  int X = 0;
  int Y = 0;

  int reddist = 0;
  int bluedist = 0;
  int pinkdist = 0;
  int orgdist = 0;
  int grndist = 0;
  int grnave = 0;

  for(int i=0; i<10000; i++){
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();
    if(spi_val%8==0){
      nocol++;
      int spi_val2 = spi_val/8;
    dist = dist + spi_val2%32;
    int spi_val3 = spi_val2/32;
    Y = Y + spi_val3%16;
    int spi_val4 = spi_val3/16;
    X= X + spi_val4;
    }
    if(spi_val%8==1){
      redcount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      reddist = reddist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==2){
      bluecount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      bluedist = bluedist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==4){
      pinkcount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      pinkdist = pinkdist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==3){
      orangecount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      orgdist = orgdist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==7){
      greencount++;
      grndist = grndist + spi_val/8;
    }
    }  /*  
  Serial.print(dist);
  Serial.print("Red: ");
  Serial.println(redcount);
  Serial.print("Blue: ");
  Serial.println(bluecount);
  Serial.print("Pink: ");
  Serial.println(pinkcount);
  Serial.print("Orange: ");
  Serial.println(orangecount);
  Serial.print("Green: ");
  Serial.println(greencount);*/
  Serial.println(orangecount);
  Serial.println(redcount);
  Serial.println(pinkcount);
  Serial.println(bluecount);
  Serial.println(greencount);
  if(greencount!=0){
  grnave = grndist/greencount;
  }
  Serial.print("Val: ");
  Serial.println(grnave);
  if (greencount > 0 && greencount < 500 ) {
    Serial.println("Alien Building");
  }
  else if(pinkcount + orangecount + redcount + bluecount + greencount < 50){
    Serial.println("No Ball");
  }
  else if(pinkcount > orangecount && pinkcount*25 >redcount  && bluecount < 25){
    Serial.println("Pink");
    Serial.println(pinkdist);
  }
  else if(orangecount > pinkcount && orangecount*15 > redcount && bluecount < 25){
    Serial.println("Orange");
    Serial.println(orgdist);
  }
  else if(redcount >= bluecount && redcount >= pinkcount && redcount >= orangecount && redcount >= greencount && redcount>0 && bluecount < 25){
    Serial.println("Red");
    Serial.println(reddist);
  }
  else if (bluecount>25 && bluecount < 1000){
    Serial.println("Blue");
    Serial.println(bluedist);
  }
  else {
    Serial.println("No Ball");
  }
  delay(1000);
}
