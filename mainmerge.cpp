#include <Arduino.h>
#include <SPI.h>
#include <drive.h>

#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS SS

SPISettings settings(100000, MSBFIRST, SPI_MODE0);

uint8_t spi_counter[6];
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;

void calcDistance();
void resetCounter();

int drivebreak = 1;
int cornerleft = 1;
int length = 10;
int currlength = -1;

int coordx = 0;
int coordy = 0;
int anglepoint = 0;

//Specify which corner rover starts and distance

Drive driveUnity;
Motors motor;

void setup()
{
  Serial.begin(9600);
  driveUnity.setup();
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  spi_returnval = 0;
}
String automove(){
  currlength++;
  if(currlength>length){
    currlength = 0;
    if(cornerleft==1){
      motor.turn_angle(90,false);
      driveUnity.forward(4);
      delay(2000);
      motor.turn_angle(90,false);
      currlength--;
    }
    else{
      motor.turn_angle(90,true);
      driveUnity.forward(4);
      delay(2000);
      motor.turn_angle(60,true);
      currlength--;
    }
    cornerleft = cornerleft * -1;
  }
  // optical_measurements();
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
  int buildingdist = 1;
  int prevbdist = 0;
  int instability = 0;

  for (int i = 0; i < 5000; i++)
  {
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();

    int col = spi_val % 8;
    spi_val = spi_val / 8;
    int pos = spi_val % 8;
    spi_val = spi_val / 8;
    int dist = spi_val;

    if (col == 6){
      driveUnity.brake();
      currlength = 0;
      cornerleft = 1;
      return "";
    }

    if (col == 0)
    {
      nocol++;
    }
    else if (col == 1)
    {
      if (dist != 0 && ((i > 1000 && abs((reddist / redcount) - dist) < 50) || i < 1000))
      {
        redcount++;
        reddist = reddist + dist;
      }
      redpos = redpos + pos;
    }
    else if (col == 2)
    {
      if (dist != 0 && ((i > 1000 && abs((bluedist / bluecount) - dist) < 50) || i < 1000))
      {
        bluecount++;
        bluedist = bluedist + dist;
      }
      bluepos = bluepos + pos;
    }
    else if (col == 3)
    {
      if (dist != 0 && ((i > 1000 && abs((orgdist / orangecount) - dist) < 50) || i < 1000))
      {
        orangecount++;
        orgdist = orgdist + dist;
      }
      orgpos = orgpos + pos;
    }
    else if (col == 4)
    {
      if (dist != 0 && ((i > 1000 && abs((pinkdist / pinkcount) - dist) < 50) || i < 1000))
      {
        pinkcount++;
        pinkdist = pinkdist + dist;
      }
      pinkpos = pinkpos + pos;
    }
    else if (col == 7)
    {
      instability = instability + abs(dist-prevbdist);
      if (dist != 0 && ((i > 1000 && abs((buildingdist / buildingcount) - dist) < 50) || i < 1000))
      {
        buildingcount++;
        buildingdist = buildingdist + dist;
      }
      prevbdist = dist;
    }
  }

  int reddistf = 2670 / (reddist / redcount);
  int orgdistf = 2670 / (orgdist / orangecount);
  int pinkdistf = 2670 / (pinkdist / pinkcount);
  int bluedistf = 2670 / (bluedist / bluecount);
  int maindistf = 1000;
  int buildingdistf = 2670/(buildingdist/buildingcount);

  /*

  Serial.print("red");
  Serial.println(redcount);
  Serial.print("orange");
  Serial.println(orangecount);
  Serial.print("pink");
  Serial.println(pinkcount);
  Serial.print("blue");
  Serial.println(bluecount);

  */
  if (buildingdistf!=2670 && buildingdistf!=2 && instability/buildingcount < 50){
    if (cornerleft==1)
    {
      driveUnity.brake();
      motor.turn_angle(90,false);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(80,true);
    }
    else if (cornerleft==0)
    {
      driveUnity.brake();
      motor.turn_angle(80,true);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(90,false);
    }
    else
    {
      driveUnity.forward(3);
    }
    return "BU;"+String(buildingdistf)+";"+String(instability/buildingcount);
  }
  if ((pinkdistf < 60) && (reddistf < 60) && redcount > orangecount && redcount * 3 > pinkcount)
  {
    Serial.println("Red Ball");
    Serial.print(pinkdistf);
    Serial.println(" cm");
    Serial.println(redpos / redcount);
    maindistf = pinkdistf;
    if (maindistf < 20 && cornerleft==1)
    {
      driveUnity.brake();
      motor.turn_angle(90,false);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(80,true);
    }
    else if (maindistf < 20 && cornerleft==0)
    {
      driveUnity.brake();
      motor.turn_angle(80,true);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(90,false);
    }
    else
    {
      driveUnity.forward(3);
    }
    return "RE"+String(2670/(buildingdist/buildingcount));
  }
  else if (pinkdistf < 60 && pinkdistf > 5)
  {
    Serial.println("Pink Ball");
    Serial.print(pinkdistf);
    Serial.println(" cm");
    Serial.println(pinkpos / pinkcount);
    maindistf = pinkdistf;
    if (maindistf < 20 && cornerleft==1)
    {
      driveUnity.brake();
      motor.turn_angle(90,false);
      driveUnity.forward(3);
      delay(3000);
      motor.turn_angle(80,true);
    }
    else if (maindistf < 20 && cornerleft==0)
    {
      driveUnity.brake();
      motor.turn_angle(80,true);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(90,false);
    }
    else
    {
      driveUnity.forward(3);
    }
    return "PI"+String(2670/(buildingdist/buildingcount));
  }
  else if (orgdistf < 60 && orangecount > 5)
  {
    Serial.println("Orange Ball");
    Serial.print(orgdistf);
    Serial.println(" cm");
    Serial.println(orgpos / orangecount);
    maindistf = orgdistf;
    if (maindistf < 20 && cornerleft==1)
    {
      driveUnity.brake();
      motor.turn_angle(90,false);
      driveUnity.forward(3);
      delay(3000);
      motor.turn_angle(80,true);
    }
    else if (maindistf < 20 && cornerleft==0)
    {
      driveUnity.brake();
      motor.turn_angle(80,true);
      driveUnity.forward(4);
      delay(3000);
      motor.turn_angle(90,false);
    }
    else
    {
      driveUnity.forward(3);
    }
    return "OA"+String(2670/(buildingdist/buildingcount));
  }
  else if (bluedistf < 60 && bluecount > 20)
  {
    Serial.println("Blue Ball or No Ball");
    Serial.print(bluedistf);
    Serial.println(" cm");
    Serial.println(bluepos / bluecount);
    Serial.println(bluecount);
    driveUnity.forward(3);
    return "BL"+String(2670/(buildingdist/buildingcount));
  }
  else
  {
    //Serial.println("No Ball");
    driveUnity.forward(3);
    return "NB"+String(2670/(buildingdist/buildingcount));
  }
  // driveUnity.forward(1);
  // delay(1000);
}
String manualmove(char decision){
  if(decision=='f'){
    driveUnity.forward(3);
  }
  else if(decision=='r'){
    motor.turn_angle(90,false);
  }
  else if(decision=='l'){
    motor.turn_angle(90,true);
  }
  else if(decision=='b'){
    driveUnity.brake();
  }
  // optical_measurements();
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
  int buildingdist = 1;

  for (int i = 0; i < 5000; i++)
  {
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();

    int col = spi_val % 8;
    spi_val = spi_val / 8;
    int pos = spi_val % 8;
    spi_val = spi_val / 8;
    int dist = spi_val;

    if (col == 6){
      driveUnity.brake();
      currlength = 0;
      cornerleft = 1;
      return "";
    }

    if (col == 0)
    {
      nocol++;
    }
    else if (col == 1)
    {
      if (dist != 0 && ((i > 1000 && abs((reddist / redcount) - dist) < 50) || i < 1000))
      {
        redcount++;
        reddist = reddist + dist;
      }
      redpos = redpos + pos;
    }
    else if (col == 2)
    {
      if (dist != 0 && ((i > 1000 && abs((bluedist / bluecount) - dist) < 50) || i < 1000))
      {
        bluecount++;
        bluedist = bluedist + dist;
      }
      bluepos = bluepos + pos;
    }
    else if (col == 3)
    {
      if (dist != 0 && ((i > 1000 && abs((orgdist / orangecount) - dist) < 50) || i < 1000))
      {
        orangecount++;
        orgdist = orgdist + dist;
      }
      orgpos = orgpos + pos;
    }
    else if (col == 4)
    {
      if (dist != 0 && ((i > 1000 && abs((pinkdist / pinkcount) - dist) < 50) || i < 1000))
      {
        pinkcount++;
        pinkdist = pinkdist + dist;
      }
      pinkpos = pinkpos + pos;
    }
    else if (col == 7)
    {
      buildingcount++;
      buildingdist = buildingdist + dist;
    }
  }

  int reddistf = 2670 / (reddist / redcount);
  int orgdistf = 2670 / (orgdist / orangecount);
  int pinkdistf = 2670 / (pinkdist / pinkcount);
  int bluedistf = 2670 / (bluedist / bluecount);
  int maindistf = 1000;

  if ((pinkdistf < 60) && (reddistf < 60) && redcount > orangecount && redcount * 3 > pinkcount)
  {
    return "RE";
  }
  else if (pinkdistf < 60 && pinkdistf > 5)
  {
    return "PI";
  }
  else if (orgdistf < 60 && orangecount > 5)
  {
    return "OA";
  }
  else if (bluedistf < 60 && bluecount > 20)
  {
    return "BL";
  }
  else
  {
    return "NB;"+String(2670/(buildingdist/buildingcount));
  }

}

void loop(){
  Serial.println(manualmove('f'));
}
