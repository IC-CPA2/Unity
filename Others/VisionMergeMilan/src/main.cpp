#include <Arduino.h>
#include <SPI.h>
#include <drive.h>
#include <Radar.h>
#include <gyroscope.h>

#include <Wire.h>
#include <MFRC522.h>
#include <iostream>
#include <WiFi.h>
#include <MPU6050_light.h>

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
GyroScope gyro; 

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
  mfrc522.PCD_Init();
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
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

float fl_1 = 0.92;
float fl_2 = 10;
float fl_3 = 0.5;
float fl_4 = 5;

void loop()
{
  /*
  hspi->beginTransaction(settings);
  digitalWrite(HSPI_SS, LOW);
  Serial.println(hspi->transfer16(spi_returnval));
  spi_returnval = 0;
  digitalWrite(HSPI_SS, HIGH);
  hspi->endTransaction();
  driveUnity.turn(90,false);
  radar.fan_detect();
  delay(500);
  */
  // Start the Comms
  Serial.println(UnityGyro.diffZ());

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
  msg = "";
  client.print("hello");
  while (i == 0)
  {
    if (!connected)
    {
      Serial.println("connection to host server failed");
      delay(1000);
      return;
    }
    connected = true;
    driveUnity.forward(speed, fl_1, fl_2, fl_3, fl_4);
    while (client.available())
    {
      char c = client.read();
      msg.concat(c);

      if (c == '>')
      {
        i = 1;
        Serial.println(msg);
      } // getting the reads from the ser

      // Serial.println(c);
      // speed = (int)c;

      // i = 1;
      // break;
    }
  }

  Serial.println("exit for loop");
  int ind_semicolon = msg.indexOf(";");
  String str_param = msg.substring(0, ind_semicolon); // get the
  speed = str_param.toInt();                          // 1
  Serial.println(speed, DEC);
  msg = msg.substring(ind_semicolon + 1, msg.length());

  // ind_semicolon = msg.indexOf(";");
  // str_param = msg.substring(0, ind_semicolon); // get the
  // int straight_speed = str_param.toInt();      // 2
  // Serial.println(straight_speed, DEC);
  // msg = msg.substring(ind_semicolon + 1, msg.length());

  // ind_semicolon = msg.indexOf(";");
  // str_param = msg.substring(0, ind_semicolon); // get the
  // int turning_speed = str_param.toInt();       // 3
  // Serial.println(turning_speed, DEC);
  // msg = msg.substring(ind_semicolon + 1, msg.length());

  Serial.print("end of ints, message is: ");

  Serial.print(msg);
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  fl_1 = str_param.toFloat();                  // this is the first float
  Serial.println("extract float");
  Serial.println(fl_1, 3); // print to 3 degrees of precision (3 d.p.)

  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  fl_2 = str_param.toFloat();                  // this is the second float
  Serial.println(fl_2, 3);                     // print to 3 degrees of precision (3 d.p.)

  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  fl_3 = str_param.toFloat();                  // this is the second float
  Serial.println(fl_3, 3);                     // print to 3 degrees of precision (3 d.p.)

  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  fl_4 = str_param.toFloat();                  // this is the second float
  Serial.println(fl_4, 3);                     // print to 3 degrees of precision (3 d.p.)

  Serial.println(speed, DEC);
  driveUnity.forward(speed, fl_1, fl_2, fl_3, fl_4);
}