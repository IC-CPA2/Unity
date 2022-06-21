#include <Arduino.h>
// #include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <iostream>
#include <WiFi.h>
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define RST_PIN 4
#define SS_PIN 2
#include <drive.h>

WiFiClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);
const char *ssid = "Milu-PC"; // the ssid field here is the name of your wifi
const char *password = "123456789";
const uint16_t port = 12001;
// const char *host = "192.168.14.148";
// const char *host = "192.168.1.76";//this is the host address of the LAN
// 192.168.137.71
const char *host = "192.168.137.1";

// total translation of the rover
double total_translation_x, total_translation_y;

Motors motors;

Drive driveUnity;

// you extract it by going ipconfig on your own desktop.
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
void setup()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  // optical_setup();
  driveUnity.setup();
  delay(2000);
}
// int total_translation_x,total_translation_y;
// Motors motors;

int straight_turn = 0;
int straight_speed = 3;
int turning_speed = 70;

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
  String msg = ""; // e.g. 0;0;3;5.1>
  connected = client.connect(host, port);
  int i = 0;

  while (i == 0)
  {
    if (!connected)
    {
      Serial.println("connection to host server failed");
      delay(1000);
      return;
    }

    if (straight_turn == 0)
    {
      driveUnity.forward(straight_speed);
    }
    else
    {
      driveUnity.turn(turning_speed, 90, true);
    }

    Serial.println("Connected to server successful!");
    client.print("Hello from ESP32!"); // send the vision data here
    // server is processing here
    connected = true;
    while (client.available())
    {
      char c = client.read();
      msg.concat(c);
      // Serial.println(c);
      if (c == '>')
      {
        i = 1;
        Serial.println(msg);
      } // getting the reads from the ser
    }
    // client.stop();
    delay(1000);
  }
  // param names: straight_turn (int), straight_speed (int), param3 (int), float fl_1 (float), float fl_2 (float)
  // float fl_3 (float), float kp (float), float ki (float), float fl_4
  Serial.println("exit for loop");
  int ind_semicolon = msg.indexOf(";");
  String str_param = msg.substring(0, ind_semicolon); // get the
  straight_turn = str_param.toInt();                  // 1
  Serial.println(straight_turn, DEC);
  msg = msg.substring(ind_semicolon + 1, msg.length());

  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  int straight_speed = str_param.toInt();      // 2
  Serial.println(straight_speed, DEC);
  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  int turning_speed = str_param.toInt();       // 3
  Serial.println(turning_speed, DEC);
  msg = msg.substring(ind_semicolon + 1, msg.length());
  Serial.print("end of ints, message is: ");

  Serial.print(msg);
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  float fl_1 = str_param.toFloat();            // this is the first float
  Serial.println("extract float");
  Serial.println(fl_1, 3); // print to 3 degrees of precision (3 d.p.)

  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  float fl_2 = str_param.toFloat();            // this is the second float
  Serial.println(fl_2, 3);                     // print to 3 degrees of precision (3 d.p.)
  msg = msg.substring(ind_semicolon + 1, msg.length());

  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  float fl_3 = str_param.toFloat();            // this is the third float
  Serial.println(fl_3, 3);                     // print to 3 degrees of precision (3 d.p.)
  msg = msg.substring(ind_semicolon + 1, msg.length());

  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  float kp = str_param.toFloat();              // this is the first float
  Serial.println(kp, 3);                       // print to 3 degrees of precision (3 d.p.)
  msg = msg.substring(ind_semicolon + 1, msg.length());

  ind_semicolon = msg.indexOf(";");
  str_param = msg.substring(0, ind_semicolon); // get the
  float ki = str_param.toFloat();              // this is the first float
  Serial.println(ki, 3);                       // print to 3 degrees of precision (3 d.p.)
  msg = msg.substring(ind_semicolon + 1, msg.length());
  ind_semicolon = msg.indexOf(">");
  str_param = msg.substring(0, ind_semicolon); // get the
  float fl_4 = str_param.toFloat();            // this is the first float
  Serial.println(fl_4, 3);                     // print to 3 degrees of precision (3 d.p.)
  msg = msg.substring(ind_semicolon + 1, msg.length());

  // delay(500);
  // optical_measurements();
  // Serial.println("Forward distance:");
  // Serial.println(driveUnity.coord_y);

  // optical_measurements();

  // // 666 coordinate_y difference translates to around 15cm of translation
  // driveUnity.forward_distance(4, 20);
  // driveUnity.turn(90, true);
  // driveUnity.forward_distance(4, 20);
  // driveUnity.turn(90, false);

  // if (driveUnity.coord_y < 66600)
  // {

  //     //   driveUnity.forward_distance(2, 1000);
  // }
  // else
  // {
  //     driveUnity.turn(90, true);
  //     driveUnity.turn(90, false);
  //     driveUnity.brake();
  //     Serial.println("Heading angle");
  //     Serial.println(roverUnity.head_angle);
  // }
}

// void setup()
// {

//     // delay to initialise everyything properly
//
// }

// void loop()
// {
//     delay(500);
//     //  optical_measurements();
//     //  Serial.println("Forward distance:");
//     //  Serial.println(driveUnity.coord_y);

//     // optical_measurements();

//     // 666 coordinate_y difference translates to around 15cm of translation
//     driveUnity.forward_distance(4, 20);
//     driveUnity.turn(90, true);
//     driveUnity.forward_distance(4, 20);
//     driveUnity.turn(90, false);

//     if (driveUnity.coord_y < 66600)
//     {

//         //   driveUnity.forward_distance(2, 1000);
//     }
//     else
//     {
//         driveUnity.turn(90, true);
//         driveUnity.turn(90, false);
//         driveUnity.brake();
//         Serial.println("Heading angle");
//         Serial.println(roverUnity.head_angle);
//     }
// }

// void loop(){

// total_translation_x,total_translation_y = optical_measurements();

//     //speed is to be between -10 and 10
//     int rover_speed = 5;

//     //translation in centimetres
//     int rover_translation = 10000;

//     //relative turning angle compared to current position
//     int rover_rotation_angle = 0;

//     //the straightness error
//     int error = 0; //calculate this based on total_translations and knowledge on the current direction

//     //motors_control(rover_speed,rover_translation,rover_rotation_angle,error);
//     if(coord_y > 10000){
//         motors.brake();
//     }else{
//         Serial.println("total translation x:");
//         Serial.println(coord_x);
//        motors.forward(3,coord_x);
//         //motors.turn(90);
//         //motors.turn(90,false);
//     }
// }
