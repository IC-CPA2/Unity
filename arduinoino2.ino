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
int coordx =0;
int coordy = 0;
int anglepoint = 0;

void setup() {
  Serial.begin(9600);
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  spi_returnval = 0;
}
int currlength=0;
int length = 20;
int retlargestbin(int arr[65]){
  int maxindex = 0;
  int maxval = 0;
  for(int i=0;i<65;i++){
    if(arr[i]>maxval){
      maxval = arr[i];
      maxindex = i;
    }
  }
  if (maxindex == 0 || maxval == 0){
    return 1000;
  }
  else{
    return 2670/(maxindex*10);
  }
}
int retstrength(int arr[65]){
  int maxindex = 0;
  int maxval = 0;
  for(int i=0;i<65;i++){
    if(arr[i]>maxval){
      maxval = arr[i];
      maxindex = i;
    }
  }
  if (maxindex == 0 || maxval == 0){
    return 0;
  }
  else{
    return maxval;
  }
}

String currentview(){
  int redbin[65] = {0};
  int bluebin[65] = {0};
  int pinkbin[65] = {0};
  int yelbin[65] = {0};
  int gbin[65] = {0};
  int dgbin[65] = {0};

  int nocol = 0;
  int redpos = 0;
  int bluepos = 0;
  int pinkpos = 0;
  int yelpos = 0;
  int gpos = 0;
  int dgpos = 0;

  int redc = 0;
  int bluec = 0;
  int pinkc = 0;
  int yelc = 0;
  int gc = 0;
  int dgc = 0;

  int buildingcount = 1;
  int buildingdist = 1;

  for (int i = 0; i < 10000; i++)
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
    int tdist = dist/10;

    if (col == 0 && pos == 0 && dist == 0){
      //driveUnity.brake();
      //currlength = 0;
      //cornerleft = 1;
    }

    if (col == 0){
      nocol++;
    }
    else if (col == 1){
      redbin[tdist]++;
      redpos = redpos + pos;
      redc++;
    }
    else if (col == 2){
      bluebin[tdist]++;
      bluepos = bluepos + pos;
      bluec++;
    }
    else if (col == 3){
      yelbin[tdist]++;
      yelpos = yelpos + pos;
      yelc++;
    }
    else if (col == 4){
      pinkbin[tdist]++;
      pinkpos = pinkpos + pos;
      pinkc++;
    }
    else if (col == 5){
      gbin[tdist]++;
      gpos = gpos + pos;
      gc++;
    }
    else if (col == 6){
      dgbin[tdist]++;
      dgpos = dgpos + pos;
      dgc++;
    }
    else if (col == 7)
    {
      buildingcount++;
      buildingdist = buildingdist + dist;
    }
  }

  int reddistf = retlargestbin(redbin);
  int yeldistf = retlargestbin(yelbin);
  int pinkdistf = retlargestbin(pinkbin);
  int bluedistf = retlargestbin(bluebin);
  int gdistf = retlargestbin(gbin);
  int dgdistf = retlargestbin(dgbin);
  int maindistf = 1000;

  /*
  return "reddist: "+String(reddistf) + "," + String(retstrength(redbin))+ ";" +
          "yeldist: "+String(yeldistf) + + "," + String(retstrength(yelbin))+";" +
          "pinkdist: "+String(pinkdistf) + + "," + String(retstrength(pinkbin))+";"+
          "bluedist: "+String(bluedistf) + + "," + String(retstrength(bluebin))+";"+
          "gdistf: "+String(gdistf) + + "," + String(retstrength(gbin))+";"+
          "dgdistf: "+String(dgdistf) + + "," + String(retstrength(dgbin))+";"+
          "buildingdist: "+String(buildingcount)+","+String(buildingdist); 
  */
  
  if((retstrength(redbin)>10)){
    if(reddistf<20){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;RAx;"+String(anglepoint);
    }
    else if(reddistf<40){
      return String(coordx)+","+String(coordy)+";T1;RAx;T3;T4;"+String(anglepoint);
    }
  }
  if((retstrength(yelbin)>10)){
    if(yeldistf<15){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;YAx;"+String(anglepoint);
    }
    else if(yeldistf<40){
      return String(coordx)+","+String(coordy)+";T1;YAx;T3;T4;"+String(anglepoint);
    }
  }
  if((retstrength(pinkbin)>20)){
    if(pinkdistf<15){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;PAx;"+String(anglepoint);
    }
    else if(pinkdistf<40){
      return String(coordx)+","+String(coordy)+";T1;PAx;T3;T4;"+String(anglepoint)+","+String(pinkdistf);
    }
  }
  if((retstrength(bluebin)>10)){
    if(bluedistf<15){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;BAx;"+String(anglepoint);
    }
    else if(bluedistf<40){
      return String(coordx)+","+String(coordy)+";T1;BAx;T3;T4;"+String(anglepoint);
    }
  }
  if((retstrength(gbin)>10)){
    if(gdistf<15){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;GAx;"+String(anglepoint);
    }
    else if(gdistf<40){
      return String(coordx)+","+String(coordy)+";T1;GAx;T3;T4;"+String(anglepoint);
    }
  }
  if((retstrength(dgbin)>10)){
    if(dgdistf<15){
      return String(coordx)+","+String(coordy)+";T1;T2;T3;DGAx;"+String(anglepoint);
    }
    else if(dgdistf<40){
      return String(coordx)+","+String(coordy)+";T1;DGAx;T3;T4;"+String(anglepoint);
    }
  }
}
void loop(){
  Serial.println(currentview());
}
