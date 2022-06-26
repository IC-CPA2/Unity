#include <Arduino.h>
#include <SPI.h>
#include <drive.h>
#include <Radar.h>

#include <Wire.h>

#include <iostream>
#include <WiFi.h>
// #include <MPU6050_light.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15

// #define SCK 18
// #define MISO 19
// #define MOSI 23
// #define CS 5


WiFiClient client;


SPISettings settings(100000, MSBFIRST, SPI_MODE0);
IPAddress gateway(192, 168, 14, 224);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
uint8_t spi_counter[6];
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
const char *ssid = "matthew";
const char *password = "12345678";
const uint16_t port = 12000;
const char *host = "192.168.137.190";

void calcDistance();
void resetCounter();
int drivebreak = 1;
int cornerleft = 1;
int length = 10;
int currlength = -1;

int coordx = 0;
int coordy = 0;
int anglepoint = 0;

// Specify which corner rover starts and distance

Drive driveUnity;
Motors motor;
Radar radar;

void initWiFi()
{
  /*
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print('.');
      delay(1000);
    }
    */
}

SPIClass *hspi = NULL;

void setup()
{
  Serial.begin(9600);
  driveUnity.setup();
  pinMode(HSPI_SS, OUTPUT);
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
  radar.setup();
  spi_returnval = 0;

  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  spi_returnval = 0;
}

char *getColor(char *string)
{
  char *token = strtok(string, ";");
  return token;
}

int getDist(String string)
{
  int index = string.indexOf(';');
  String sub_SC = string.substring(0, index);
  string.remove(0, sub_SC.length() + 1);
  int d = string.indexOf(';');
  String sub_SD = string.substring(0, d);
  return sub_SD.toInt();
}

int getPos(char *string)
{
  int pos = 0;
  char *token = strtok(string, ";");
  char arr[3][50];
  int i = 0;

  while (token != NULL)
  {
    strcpy(arr[i], token);
    token = strtok(NULL, ";");
    i++;
  }
  pos = atoi(arr[2]);
  return pos;
}

int retlargestbin(int arr[65])
{
  int maxindex = 0;
  int maxval = 0;
  for (int i = 0; i < 65; i++)
  {
    if (arr[i] > maxval)
    {
      maxval = arr[i];
      maxindex = i;
    }
  }
  if (maxindex == 0 || maxval == 0)
  {
    return 1000;
  }
  else
  {
    return 2670 / (maxindex * 10);
  }
}

int totlum()
{
  int tot = 1;
  int c = 1;
  for (int i = 0; i < 1000; i++)
  {
    hspi->beginTransaction(settings);
    digitalWrite(HSPI_SS, LOW);
    spi_val = hspi->transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(HSPI_SS, HIGH);
    hspi->endTransaction();

    int col = spi_val % 8;
    spi_val = spi_val / 8;
    int pos = spi_val % 8;
    spi_val = spi_val / 8;
    int dist = spi_val;

    if (col == 0)
    {
      c++;
      tot = tot + dist;
    }
  }
  return tot / c;
}

int retstrength(int arr[65])
{
  int maxindex = 0;
  int maxval = 0;
  for (int i = 0; i < 65; i++)
  {
    if (arr[i] > maxval)
    {
      maxval = arr[i];
      maxindex = i;
    }
  }
  if (maxindex == 0 || maxval == 0)
  {
    return 0;
  }
  else
  {
    return maxval;
  }
}

String currentview()
{
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

  for (int i = 0; i < 1000; i++)
  {
    hspi->beginTransaction(settings);
    digitalWrite(HSPI_SS, LOW);
    spi_val = hspi->transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(HSPI_SS, HIGH);
    hspi->endTransaction();

    int col = spi_val % 8;
    spi_val = spi_val / 8;
    int pos = spi_val % 8;
    spi_val = spi_val / 8;
    int dist = spi_val;
    int tdist = dist / 10;

    if (col == 0 && pos == 0 && dist == 0)
    {
      // driveUnity.brake();
      currlength = 0;
      // cornerleft = 1;
    }

    if (col == 0)
    {
      nocol++;
    }
    else if (col == 1)
    {
      redbin[tdist]++;
      redpos = redpos + pos;
      redc++;
    }
    else if (col == 2)
    {
      bluebin[tdist]++;
      bluepos = bluepos + pos;
      bluec++;
    }
    else if (col == 3)
    {
      yelbin[tdist]++;
      yelpos = yelpos + pos;
      yelc++;
    }
    else if (col == 4)
    {
      pinkbin[tdist]++;
      pinkpos = pinkpos + pos;
      pinkc++;
    }
    else if (col == 5 && tdist <= 65)
    {
      gbin[tdist]++;
      gpos = gpos + pos;
      gc++;
    }
    else if (col == 6 && tdist <= 65)
    {
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
  return "reddist: "+String(reddistf) + "," + String(retstrength(redbin))+ ","+ String(redc)+";"
          "yeldist: "+String(yeldistf) + + "," + String(retstrength(yelbin))+ ","+ String(yelc)+";" +
          "pinkdist: "+String(pinkdistf) + + "," + String(retstrength(pinkbin))+ ","+ String(pinkc)+";"+
          "bluedist: "+String(bluedistf) + + "," + String(retstrength(bluebin))+ ","+ String(bluec)+";"+
          "gdistf: "+String(gdistf) + + "," + String(retstrength(gbin))+ ","+ String(gc)+";"+
          "dgdistf: "+String(dgdistf) + + "," + String(retstrength(dgbin))+ ","+ String(dgc)+";"+
          "buildingdist: "+String(buildingcount)+","+String(buildingdist);
  */
  if (retstrength(redbin) > 5 && redc > 20 && retstrength(pinkbin) < 100)
  {
    return "r;" + String(reddistf) + ";" + String(redpos / redc);
  }
  else if (retstrength(yelbin) > 10 && retstrength(pinkbin) < 100)
  {
    return "y;" + String(yeldistf) + ";" + String(yelpos / yelc);
  }
  else if (retstrength(pinkbin) > 20)
  {
    return "p;" + String(pinkdistf) + ";" + String(pinkpos / pinkc);
  }
  else if (retstrength(bluebin) > 10 && bluec >= 10)
  {
    return "b;" + String(bluedistf) + ";" + String(bluepos / bluec);
  }
  else if (retstrength(dgbin) > 10 && dgc > 15)
  {
    return "dg;" + String(dgdistf) + ";" + String(dgpos / dgc);
  }
  else if (retstrength(gbin) > 10 && gc > 15)
  {
    return "lg;" + String(gdistf) + ";" + String(gpos / gc);
  }
  else
  {
    return "nb;1000;1000";
  }
}

void loop()
{
  // Serial.println(totlum());
  // Serial.println(getDist(currentview()));
  // driveUnity.turn(90, false);
  // delay(1000);
  // driveUnity.turn(90, true);
  // delay(1000);
  
  double current_angle = gyro.currentangle();

  // Serial.println(current_angle);
  // delay(500);
  //Serial.println(totlum());
  //Serial.println(getDist(currentview()));
  //driveUnity.forward_distance(3,20);
  Serial.println("Turn right");
  driveUnity.turn(90, false);
  delay(1000);
  Serial.println("Turn left");
  driveUnity.turn(90, true);
  delay(1000);

  /*
  Serial.println(currentview());
  String view = currentview();
  char val[100];
  view.toCharArray(val,100);
  Serial.println(view);
  Serial.println(val);
  Serial.println(getPos(val));
  Serial.println(getDist(view));
  Serial.println(getColor(val));
  */

  /*
  if(totlum()<70){
    Serial.println("wall");
    driveUnity.turn(90,false);
    driveUnity.forward(3);
    delay(3000);
    driveUnity.turn(90,false);
  }
  if(getDist(currentview())<25){
    Serial.println("avoiding");
    driveUnity.turn(90,false);
    driveUnity.forward(3);
    delay(3000);
    driveUnity.turn(90,true);
  }
  else{
    driveUnity.forward(3);
  }
  */

  /*
  String view = currentview();
  char * val;
  view.toCharArray(val,10);

  if(getDist(val)<20){
    driveUnity.turn(90,false);
    driveUnity.forward(3);
    delay(3000);
    driveUnity.turn(90,true);
  }

  unsigned long previousMillis = 0;
  unsigned long interval = 30000;
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)){
    Serial.print(millis());
    Serial.println("Need To Reconnect....");
    WiFi.disconnect();
    initWiFi();
    previousMillis = currentMillis;
  }
  bool connected;
  std::string msg;
  connected = client.connect(host, port);
  int i = 0;
  client.print("hello");
  while (i==0){
    if (!connected)
    {
      Serial.println("connection to host server failed");
      delay(1000);
      return;
    }
    connected = true;
    while (client.available())
    {
      char c = client.read();
      Serial.println(c);
      driveUnity.turn(90,false);
      i=1;
      break;
    }
  }
  */
}