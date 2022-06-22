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
#define RST_PIN 27
#define SS_PIN 26

WiFiClient client;

// Testing
MFRC522 mfrc522(SS_PIN, RST_PIN);
SPISettings settings(100000, MSBFIRST, SPI_MODE0);
// IPAddress gateway(192, 168, 14, 224);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);
uint8_t spi_counter[6];
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
const char *ssid = "Milu-PC";
const char *password = "123456789";
const uint16_t port = 12000;
const char *host = "192.168.137.1";

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
  radar.setup();
  spi_returnval = 0;
  // mfrc522.PCD_Init();
  // initWiFi();
  // Serial.print("RRSI: ");
  // Serial.println(WiFi.RSSI());
  spi_returnval = 0;
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

int speed = 3;

String msg;

float fl_1 = 0.96;
float fl_2 = 10;
float fl_3 = 0.5;
float fl_4 = 7;

bool radarDetected = false;

void loop()
{
  // driveUnity.forward(speed, fl_1, fl_2, fl_3, fl_4);

  if (!radarDetected)
  {
    radarDetected = radar.fan_detect();
    driveUnity.forward(speed, fl_1, fl_2, fl_3, fl_4);
  }
  else
  {
    driveUnity.brake();
  }
  // driveUnity.forward_distance(speed, 80, fl_1, fl_2, fl_3, fl_4);
  // driveUnity.turn(90, true);
  // driveUnity.forward_distance(speed, 40, fl_1, fl_2, fl_3, fl_4);
  // driveUnity.turn(90, true);
  // driveUnity.forward_distance(speed, 80, fl_1, fl_2, fl_3, fl_4);
  // driveUnity.turn(90, false);
  // driveUnity.forward_distance(speed, 40, fl_1, fl_2, fl_3, fl_4);
  // driveUnity.turn(90, false);
  // driveUnity.forward_distance(speed, 80, fl_1, fl_2, fl_3, fl_4);
}