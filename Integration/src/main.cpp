#include <Arduino.h>
#include <SPI.h>
#include <drive.h>
#include <Radar.h>

#include <Wire.h>
#include <MFRC522.h>
#include <iostream>
#include <WiFi.h>

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
// #define RST_PIN 27
// #define SS_PIN 26

WiFiClient client;

// MFRC522 mfrc522(SS_PIN, RST_PIN);
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
const char *password = "123456789";
const uint16_t port = 12000;
const char *host = "192.168.137.121";

String msgs;

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

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
}

SPIClass *hspi = NULL;

void setup()
{
  Serial.begin(9600);
  driveUnity.setup();
  pinMode(HSPI_SS, OUTPUT);
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  spi_returnval = 0;
  //mfrc522.PCD_Init();
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  spi_returnval = 0;
  msgs = "start";
  driveUnity.brake();
  delay(3000);
  roverUnity.tile_x = 0;
  roverUnity.tile_y = 0;
  roverUnity.required_head_angle_unbounded = 0;
  roverUnity.head_angle_unbounded = 0;
  roverUnity.head_angle = 0;
  roverUnity.required_head_angle = 0;
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

String message(String col, int distance, int position, int coordx, int coordy, int angle)
{
  String result;
  String color;
  if (col != "nb")
  {
    if (position <= 1)
    {
      color = col + "1";
      result = String(coordx) + "," + String(coordy) + ";" + color + ";" + "T2" + ";" + "T3" + ";" + "T4" + ";" + String(angle);
    }
    else if (position == 2)
    {
      color = col + "2";
      result = String(coordx) + "," + String(coordy) + ";" + "T1" + ";" + color + ";" + "T3" + ";" + "T4" + ";" + String(angle);
    }
    else if (position == 3)
    {
      color = col + "3";
      result = String(coordx) + "," + String(coordy) + ";" + "T1" + ";" + "T2" + ";" + color + ";" + "T4" + ";" + String(angle);
    }
    else if (position >= 4)
    {
      color = col + "4";
      result = String(coordx) + "," + String(coordy) + ";" + "T1" + ";" + "T2" + ";" + "T3" + ";" + color + ";" + String(angle);
    }
  }
  else
  {
    result = String(coordx) + "," + String(coordy) + ";T1;T2;T3;T4;" + String(angle);
  }
  return result;
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

  int blackc = 0;
  int whitec = 0;

  int buildingcount = 1;
  int buildingdist = 1;

  for (int i = 0; i < 2000; i++)
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
      if (pos == 1)
      {
        whitec++;
      }
      else if (pos == 0)
      {
        blackc++;
      }
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
  if (blackc > 30 && whitec > 30)
  {
    return "bd;10;10";
  }
  if (retstrength(redbin) > 5 && redc > 5 && retstrength(pinkbin) < 20)
  {
    return "r;" + String(reddistf) + ";" + String(redpos / redc);
  }
  else if (retstrength(yelbin) > 3 && retstrength(pinkbin) < 20)
  {
    return "y;" + String(yeldistf) + ";" + String(yelpos / yelc);
  }
  else if (retstrength(pinkbin) > 5)
  {
    return "p;" + String(pinkdistf) + ";" + String(pinkpos / pinkc);
  }
  else if (retstrength(bluebin) > 3 && bluec >= 3)
  {
    return "b;" + String(bluedistf) + ";" + String(bluepos / bluec);
  }
  else if (retstrength(dgbin) > 3 && dgc > 4)
  {
    return "dg;" + String(dgdistf) + ";" + String(dgpos / dgc);
  }
  else if (retstrength(gbin) > 3 && gc > 4 && gdistf > 10)
  {
    return "lg;" + String(gdistf) + ";" + String(gpos / gc);
  }
  else
  {
    return "nb";
  }
}

int cnt = 0;
int start = -1;
int wallside = start;

// definining motor parameters

float fl_1 = 0.92;
float fl_2 = 10;
float fl_3 = 0.5;
float fl_4 = 5;

bool stop = 0;

int speed = 3;

int angle_to_turn = 90;

int lengthfield = 1;

void loop()
{
  unsigned long previousMillis = 0;
  unsigned long interval = 30000;
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
  {
    Serial.print(millis());
    Serial.println("Need To Reconnect....");
    WiFi.disconnect();
    initWiFi();
    previousMillis = currentMillis;
  }
  bool connected;
  connected = client.connect(host, port);
  int i = 0;
  Serial.println(msgs);
  client.print(msgs + ";" + String(roverUnity.tile_x) + ";" + String(roverUnity.tile_y) + ";" + String(roverUnity.head_angle));
  //client.print(msgs + ";" + String(-roverUnity.tile_y) + ";" + String(roverUnity.tile_x) + ";" + String(roverUnity.head_angle));
  while (i == 0)
  {
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
      if (c == 'A' && stop == 0)
      {
        while (1)
        {
          String tview = currentview();
          int tl = totlum();
          Serial.println(tview);
          Serial.println(totlum());
          if (tl < 100)
          {
            if (abs(coordx) > lengthfield && ((wallside == -1 && start == 1)||(wallside == 1 && start == -1)))
            {
              driveUnity.turn(angle_to_turn, start == 1);
              while (totlum() > 100)
              {
                driveUnity.forward(speed);
              }
              driveUnity.brake();
              msgs = "wall1," + String(tl);
              stop = 1;
              break;
            }
            else if (abs(coordx) > lengthfield && ((wallside == 1 && start == 1)||(wallside == -1 && start == -1)))
            {
              driveUnity.turn(angle_to_turn, false);
              Serial.println("Required unbounded, unbounded angle");
              Serial.print(roverUnity.required_head_angle_unbounded);
              Serial.print(",");
              Serial.print(roverUnity.head_angle_unbounded);
              Serial.println("----------");
              Serial.println("Required, actual head angle");
              Serial.print(roverUnity.required_head_angle);
              Serial.print(",");
              Serial.print(roverUnity.head_angle);
              Serial.println("----------");
              driveUnity.turn(angle_to_turn, false);
              Serial.println("Required unbounded, unbounded angle");
              Serial.print(roverUnity.required_head_angle_unbounded);
              Serial.print(",");
              Serial.print(roverUnity.head_angle_unbounded);
              Serial.println("----------");
              Serial.println("Required, actual head angle");
              Serial.print(roverUnity.required_head_angle);
              Serial.print(",");
              Serial.print(roverUnity.head_angle);
              Serial.println("----------");
              while (totlum() > 100)
              {
                driveUnity.forward(speed);
              }
              driveUnity.brake();
              driveUnity.turn(angle_to_turn, start == 1);
              Serial.println("Required unbounded, unbounded angle");
              Serial.print(roverUnity.required_head_angle_unbounded);
              Serial.print(",");
              Serial.print(roverUnity.head_angle_unbounded);
              Serial.println("----------");
              Serial.println("Required, actual head angle");
              Serial.print(roverUnity.required_head_angle);
              Serial.print(",");
              Serial.print(roverUnity.head_angle);
              Serial.println("----------");
              while (totlum() > 100)
              {
                driveUnity.forward(speed);
              }
              msgs = "wall2," + String(tl);
              stop = 1;
              break;
            }
            else
            {
              Serial.println("Normal Wall");
              Serial.print("LUM: ");
              Serial.println(totlum());
              wallside = wallside * -1;
              driveUnity.brake();
              driveUnity.turn(angle_to_turn, wallside != 1);
              Serial.println("Required unbounded, unbounded angle");
              Serial.print(roverUnity.required_head_angle_unbounded);
              Serial.print(",");
              Serial.print(roverUnity.head_angle_unbounded);
              Serial.println("----------");
              Serial.println("Required, actual head angle");
              Serial.print(roverUnity.required_head_angle);
              Serial.print(",");
              Serial.print(roverUnity.head_angle);
              Serial.println("----------");
              driveUnity.forward_distance(speed, 20);
              coordx++;
              driveUnity.turn(angle_to_turn, wallside != 1);
              Serial.println("Required unbounded, unbounded angle");
              Serial.print(roverUnity.required_head_angle_unbounded);
              Serial.print(",");
              Serial.print(roverUnity.head_angle_unbounded);
              Serial.println("----------");
              Serial.println("Required, actual head angle");
              Serial.print(roverUnity.required_head_angle);
              Serial.print(",");
              Serial.print(roverUnity.head_angle);
              Serial.println("----------");
              msgs = "wall," + String(tl);
              break;
            }
          }
          if (tview == "nb" || getDist(tview) > 30)
          {
            driveUnity.forward(speed);
            coordy = coordy + wallside;
          }
          else
          {
            Serial.println("Obstruction");
            driveUnity.brake();
            driveUnity.turn(angle_to_turn, wallside == 1);
            Serial.println("Required unbounded, unbounded angle");
            Serial.print(roverUnity.required_head_angle_unbounded);
            Serial.print(",");
            Serial.print(roverUnity.head_angle_unbounded);
            Serial.println("----------");
            Serial.println("Required, actual head angle");
            Serial.print(roverUnity.required_head_angle);
            Serial.print(",");
            Serial.print(roverUnity.head_angle);
            Serial.println("----------");
            driveUnity.forward_distance(speed, 20);
            coordx++;
            bool turningLeft = wallside == -1;
            driveUnity.turn(angle_to_turn, turningLeft);
            Serial.println("Required unbounded, unbounded angle");
            Serial.print(roverUnity.required_head_angle_unbounded);
            Serial.print(",");
            Serial.print(roverUnity.head_angle_unbounded);
            Serial.println("----------");
            Serial.println("Required, actual head angle");
            Serial.print(roverUnity.required_head_angle);
            Serial.print(",");
            Serial.print(roverUnity.head_angle);
            Serial.println("----------");
            msgs = tview;
            break;
          }
        }
      }
      i = 1;
      break;
    }
  }
}