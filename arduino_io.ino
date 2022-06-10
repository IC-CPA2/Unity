#include <SPI.h>
//#include <main_motor.h>
//#include <main_sensor.h>

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

int drivebreak = 1;

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

  int reddist = 1;
  int bluedist = 1;
  int pinkdist = 1;
  int orgdist = 1;

  int redpos = 0;
  int bluepos = 0;
  int pinkpos = 0;
  int orgpos = 0;
  
  int buildingcount = 1;
  int buildingdist = 0;

  for(int i=0; i<5000; i++){
    
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
      if(dist!=0 && ((i>1000 && abs((reddist/redcount)-dist)<50)||i<1000)){
        redcount++;
        reddist = reddist + dist;
      }
      redpos = redpos + pos;
    }
    else if(col==2){
      if(dist!=0 && ((i>1000 && abs((bluedist/bluecount)-dist)<50)||i<1000)){
        bluecount++;
        bluedist = bluedist + dist;
      }
      bluepos = bluepos + pos;
    }
    else if(col==3){
      if(dist!=0 && ((i>1000 && abs((orgdist/orangecount)-dist)<50)||i<1000)){
        orangecount++;
        orgdist = orgdist + dist;
      }
      orgpos = orgpos + pos;
    }
    else if(col==4){
      if(dist!=0 && ((i>1000 && abs((pinkdist/pinkcount)-dist)<50)||i<1000)){
        pinkcount++;
        pinkdist=pinkdist+dist;
      }
      pinkpos = pinkpos + pos;
    }
    else if(col==7){
      buildingcount++;
      buildingdist = buildingdist + dist;
    }
  }

  int reddistf = 2670/(reddist/redcount);
  int orgdistf = 2670/(orgdist/orangecount);
  int pinkdistf = 2670/(pinkdist/pinkcount);
  int bluedistf = 2670/(bluedist/bluecount);
  int maindistf = 1000;

  
  Serial.print("red");
  Serial.println(redcount);
  Serial.print("orange");
  Serial.println(orangecount);
  Serial.print("pink");
  Serial.println(pinkcount);
  Serial.print("blue");
  Serial.println(bluecount);
  
  
  if((pinkdistf < 60) && (reddistf < 60) && redcount>orangecount && redcount * 3 > pinkcount){
    Serial.println("Red Ball");
    Serial.print(pinkdistf);
    Serial.println(" cm");
    Serial.println(redpos/redcount);
    maindistf = pinkdistf;
  }
  else if(pinkdistf < 60 && pinkdistf > 5){
    Serial.println("Pink Ball");
    Serial.print(pinkdistf);
    Serial.println(" cm");
    Serial.println(pinkpos/pinkcount);
    maindistf = pinkdistf;
  }
  else if(orgdistf < 60 && orangecount > 5){
    Serial.println("Orange Ball");
    Serial.print(orgdistf);
    Serial.println(" cm");
    Serial.println(orgpos/orangecount);
    maindistf = orgdistf;
  }
  else if(bluedistf < 60 && bluecount > 20){
    Serial.println("Blue Ball or No Ball");
    Serial.print(bluedistf);
    Serial.println(" cm");
    Serial.println(bluepos/bluecount);
    Serial.println(bluecount);
  }
  else {
    Serial.println("No Ball");
  }/*
  if(maindistf < 10){
    motors.brake();
    motors.turn(90,false);
  }
  else{
     motors.forward(2,0);
 }*/
}
